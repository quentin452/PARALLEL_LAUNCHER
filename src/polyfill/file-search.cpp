#include "src/polyfill/file-search.hpp"

#ifdef _WIN32

#include <windows.h>
#include <fileapi.h>
#include <atomic>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>

struct SearchOptions {
	const string pattern;
	ubyte maxDepth;
	bool followSymlinks;
	bool ignoreHidden;
};

struct SearchTaskInfo {
	const SearchOptions *options;
	fs::path directory;
	ubyte depth;
};

struct INode {
	fs::path path;
	bool isDirectory;
	bool isHidden;
	bool isSymlink;
};

static inline INode toINode(
	const fs::path &parentPath,
	const WIN32_FIND_DATAW *wfData
) {
	return INode{
		/* path */ parentPath / wfData->cFileName,
		/* isDirectory */ (wfData->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0,
		/* isHidden */ wfData->cFileName[0] == '.' || (wfData->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN),
		/* isSymlink */ (wfData->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) && (wfData->dwReserved0 & IO_REPARSE_TAG_SYMLINK)
	};
}

static inline bool validNode( const wchar_t *name ) {
	if( name == nullptr || name[0] == '\0' ) return false;
	if( name[0] != '.' ) return true;
	if( name[1] == '\0' ) return false;
	if( name[1] == '.' && name[2] == '\0' ) return false;
	return true;
}

static std::vector<INode> getMatchedNodes(
	HANDLE searchHandle,
	WIN32_FIND_DATAW *nodeData,
	const fs::path &parentPath
) {
	if( searchHandle == INVALID_HANDLE_VALUE ) {
		return std::vector<INode>();
	}

	std::vector<INode> nodes;
	if( validNode( nodeData->cFileName ) ) {
		nodes.push_back( toINode( parentPath, nodeData ) );
	}

	while( FindNextFileW( searchHandle, nodeData ) ) {
		if( validNode( nodeData->cFileName ) ) {
			nodes.push_back( toINode( parentPath, nodeData ) );
		}
	}

	FindClose( searchHandle );
	return nodes;
}

static void performSearchTask(
	const SearchTaskInfo &args,
	std::queue<SearchTaskInfo> &taskQueue,
	std::vector<fs::path> &matches,
	std::mutex &taskQueueLock,
	std::mutex &matchesLock
) {
	WIN32_FIND_DATAW nodeData;
	const string searchPath = (args.directory / args.options->pattern).u8string();

	/* Get folders */
	if( args.depth < args.options->maxDepth ) {
		HANDLE searchHandle = FindFirstFileExW(
			(args.directory / L"*").wstring().c_str(),
			FindExInfoBasic,
			&nodeData,
			FindExSearchLimitToDirectories,
			nullptr,
			FIND_FIRST_EX_LARGE_FETCH
		);
		const std::vector<INode> folders = getMatchedNodes( searchHandle, &nodeData, args.directory );

		const std::lock_guard<std::mutex> lock( taskQueueLock );
		for( const INode &folder : folders ) {
			if( !folder.isDirectory ) continue;
			if( folder.isHidden && args.options->ignoreHidden ) continue;
			if( folder.isSymlink && !args.options->followSymlinks ) continue;

			taskQueue.push( SearchTaskInfo{
				args.options,
				folder.path,
				(ubyte)(args.depth + 1)
			});
		}
	}

	/* Get matching files */ {
		HANDLE searchHandle = FindFirstFileExW(
			(args.directory / args.options->pattern).wstring().c_str(),
			FindExInfoBasic,
			&nodeData,
			FindExSearchNameMatch,
			nullptr,
			FIND_FIRST_EX_LARGE_FETCH
		);
		const std::vector<INode> files = getMatchedNodes( searchHandle, &nodeData, args.directory );

		const std::lock_guard<std::mutex> lock( matchesLock );
		for( const INode &file : files ) {
			if( file.isDirectory ) continue;
			if( file.isHidden && args.options->ignoreHidden ) continue;

			matches.push_back( file.path );
		}
	}

}

static void recursiveSearchWorker(
	std::condition_variable &dispatcher,
	volatile int &activeWorkers,
	std::queue<SearchTaskInfo> &taskQueue,
	std::vector<fs::path> &matches,
	std::mutex &tasksMutex,
	std::mutex &matchesMutex,
	std::mutex &workerMutex,
	CancellationToken &cancellationToken
) {
	std::unique_lock<std::mutex> lock( workerMutex );
	while( true ) {
		std::unique_lock<std::mutex> taskQueueLock( tasksMutex );
		while( taskQueue.empty() || cancellationToken.isCancelled() ) {
			if( activeWorkers == 0 || cancellationToken.isCancelled() ) {
				taskQueueLock.unlock();
				lock.unlock();
				dispatcher.notify_all();
				return;
			}

			taskQueueLock.unlock();
			dispatcher.wait( lock );
			taskQueueLock.lock();
		}

		SearchTaskInfo task = taskQueue.front();
		taskQueue.pop();

		taskQueueLock.unlock();
		activeWorkers++;
		lock.unlock();

		performSearchTask(
			task,
			taskQueue,
			matches,
			tasksMutex,
			matchesMutex
		);

		dispatcher.notify_all();
		lock.lock();
		activeWorkers--;
	}
}


std::vector<fs::path> FileSearch::find(
	const fs::path &directory,
	const string &pattern,
	ubyte maxDepth,
	bool followSymlinks,
	bool ignoreHidden,
	CancellationToken &cancellationToken
) {
	const SearchOptions options = {
		std::move( pattern ),
		maxDepth,
		followSymlinks,
		ignoreHidden
	};

	uint numWorkers = std::thread::hardware_concurrency();
	if( numWorkers < 4 ) {
		numWorkers = 4;
	} else if( numWorkers > 16 ) {
		numWorkers = 16;
	}

	std::vector<fs::path> matches;
	std::queue<SearchTaskInfo> tasks;
	tasks.push( SearchTaskInfo{
		&options,
		directory,
		1
	});

	std::mutex matchesMutex;
	std::mutex tasksMutex;
	std::mutex workerMutex;

	std::condition_variable dispatcher;
	volatile int activeWorkers = 0;

	std::vector<std::thread> workers;
	workers.reserve( numWorkers );
	for( uint i = 0; i < numWorkers; i++ ) {
		workers.emplace_back( std::thread(
			recursiveSearchWorker,
			std::ref( dispatcher ),
			std::ref( activeWorkers ),
			std::ref( tasks ),
			std::ref( matches ),
			std::ref( tasksMutex ),
			std::ref( matchesMutex ),
			std::ref( workerMutex ),
			std::ref( cancellationToken )
		));
	}

	dispatcher.notify_one();
	for( uint i = 0; i < numWorkers; i++ ) {
		workers[i].join();
	}

	return matches;
}

#else

#include "src/polyfill/process.hpp"
#include "src/core/numeric-string.hpp"
#include <cstdio>
#include <unistd.h>
#include <signal.h>
#include <spawn.h>
#include <poll.h>
#include <fcntl.h>
#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#else
#include <wait.h>
#endif

static inline string execAndRead(
	const char *program,
	const std::vector<string> &args,
	CancellationToken &cancellationToken
) {
	string output = "";

	int comms[2];
	if( pipe( comms ) != 0 ) {
		return output;
	}

	const char *argv[args.size() + 2];
	argv[0] = string( program ).c_str();
	for( size_t i = 0; i < args.size(); i++ ) {
		argv[i+1] = args[i].c_str();
	}
	argv[args.size()+1] = nullptr;

	posix_spawn_file_actions_t actions;
	posix_spawn_file_actions_init( &actions );
	posix_spawn_file_actions_addclose( &actions, comms[0] );
	posix_spawn_file_actions_adddup2( &actions, comms[1], STDOUT_FILENO );
	posix_spawn_file_actions_addclose( &actions, comms[1] );
	posix_spawn_file_actions_addopen( &actions, STDERR_FILENO, "/dev/null", O_WRONLY, 0 );

	pid_t pid;
	int exitCode;
	if( posix_spawnp( &pid, program, &actions, nullptr, const_cast<char* const*>( argv ), environ ) != 0 ) {
		close( comms[1] );
		close( comms[0] );
		posix_spawn_file_actions_destroy( &actions );
		return output;
	}
	close( comms[1] );

	char buffer[1024];
	while( !cancellationToken.isCancelled() ) {
		pollfd fd = {
			comms[0],
			POLLIN | POLLHUP,
			0
		};

		const int status = poll( &fd, 1, 20 );
		if( status < 0 ) break;
		if( status == 0 ) continue;

		const size_t bytesRead = read( comms[0], buffer, 1023 );
		if( bytesRead == 0 ) {
			close( comms[0] );
			posix_spawn_file_actions_destroy( &actions );
			waitpid( pid, &exitCode, 0 );
			return output;
		}

		buffer[bytesRead] = '\0';
		output += buffer;
	}

	// cancelled
	close( comms[0] );
	posix_spawn_file_actions_destroy( &actions );
	kill( pid, SIGTERM );
	waitpid( pid, &exitCode, 0 );
	return "";
}

std::vector<fs::path> FileSearch::find(
	const fs::path &directory,
	const string &pattern,
	ubyte maxDepth,
	bool followSymlinks,
	bool ignoreHidden,
	CancellationToken &cancellationToken
) {
	std::vector<string> findArgs;
	findArgs.reserve( ignoreHidden ? 11 : 7 );

	findArgs.push_back( followSymlinks ? "-L" : "-H" );
	findArgs.push_back( directory.u8string() );
	findArgs.push_back( "-maxdepth" );
		findArgs.push_back( Number::toString( maxDepth ) );
	if( ignoreHidden ) {
		findArgs.push_back( "-name" );
			findArgs.push_back( ".?*" );
		findArgs.push_back( "-prune" );
		findArgs.push_back( "-o" );
	}
	findArgs.push_back( "-iname" );
		findArgs.push_back( pattern );
	findArgs.push_back( "-print" );

	const string matches = execAndRead( "find", findArgs, cancellationToken );

	std::vector<fs::path> filePaths;
	size_t lineStart = 0;
	for( int i = 0; lineStart < matches.length(); i++ ) {
		size_t lineEnd = matches.find( '\n', lineStart );
		if( lineEnd == string::npos ) {
			lineEnd = matches.length();
		}

		const fs::path filePath( matches.substr( lineStart, lineEnd - lineStart ) );
		if( !filePath.empty() && fs::isRegularFileSafe( filePath ) ) {
			filePaths.push_back( filePath );
		}

		lineStart = lineEnd + 1;
	}

	return filePaths;
}

#endif
