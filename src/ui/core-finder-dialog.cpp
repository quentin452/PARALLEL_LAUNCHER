#include "src/ui/core-finder-dialog.hpp"

#include <QMessageBox>
#include "src/core/file-controller.hpp"

AbstractCoreFinderDialog::AbstractCoreFinderDialog() :
	QProgressDialog( nullptr ),
	m_dialogOpen( new bool( false ) )
{
	setLabelText( tr( "Searching for latest core version..." ) );
	setMinimum( 0 );
	setMaximum( 0 );
}

void AbstractCoreFinderDialog::closeEvent( QCloseEvent *event ) {
	*m_dialogOpen = false;
	QProgressDialog::closeEvent( event );
}

static const string s_fallbackDownloadUrlBase = ""
	"http://buildbot.libretro.com/nightly/"
#ifdef _WIN32
	#ifdef _WIN64
		"windows/x86_64"
	#else
		"windows/x86"
	#endif
#elif __APPLE__
	/* TODO: support x86 and arm64 */
	"apple/osx/x86_64"
#else
	#ifdef _LP64
		"linux/x86_64"
	#else
		"linux/x86"
	#endif
#endif
	"/latest/";

#ifdef _WIN32
	#define LIBRARY_EXT ".dll"
#elif __APPLE__
	#define LIBRARY_EXT ".dylib"
#else
	#define LIBRARY_EXT ".so"
#endif

#ifndef __APPLE__
void MupenCoreFinderDialog::showEvent( QShowEvent *event ) {
	std::shared_ptr<bool> dialogOpen = m_dialogOpen;
	*dialogOpen = true;

	string branch = "master";
	if( FileController::loadAppSettings().mupenDevBranch ) {
		branch = "develop";
	}

	m_build.reset();
	MupenCoreBuilds::getLastKnownGood(
		branch,
		"",
		[this,dialogOpen](const CoreBuild &build) {
			// Don't need locks here because queued connections are used
			if( *dialogOpen ) {
				this->m_build = build;
				this->close();
			}
		},
		[this,dialogOpen](){
			if( *dialogOpen ) {
				this->m_build = CoreBuild {
					CommitInfo{ "", "", "" },
					s_fallbackDownloadUrlBase + ("mupen64plus_next_libretro" LIBRARY_EXT ".zip")
				};
				this->close();
			}
		}
	);

	QProgressDialog::showEvent( event );
}
#endif

void ParallelCoreFinderDialog::showEvent( QShowEvent *event ) {
	std::shared_ptr<bool> dialogOpen = m_dialogOpen;
	*dialogOpen = true;

	m_build.reset();
	ParallelCoreBuilds::getLatest(
		[this,dialogOpen](const ParallelCoreVersion &version) {
			if( *dialogOpen ) {
				this->m_build = version;
				this->close();
			}
		},
		[this,dialogOpen]() {
			if( *dialogOpen ) this->close();
		}
	);

	QProgressDialog::showEvent( event );
}
