#include "src/polyfill/process.hpp"

#include "src/polyfill/crash.hpp"
#include <thread>

#ifdef _WIN32
#include "src/polyfill/windows/unicode.hpp"
#include <shlobj.h>
#include <windows.h>
#include <winnt.h>

#define m_status static_cast<WindowsProcessData *>(m_implData.get())

struct WindowsProcessData {
  PROCESS_INFORMATION info;
  _STARTUPINFOW si;
  std::thread thread;
  int64 exitCode;
};
#else
#include "src/core/numeric-string.hpp"
#include <spawn.h>
#include <sys/wait.h>
#include <unistd.h>
#ifdef __APPLE__
#include <crt_externs.h>
#define environ (*_NSGetEnviron())
#endif

#define m_status static_cast<PosixProcessData *>(m_implData.get())

struct PosixProcessData {
  pid_t pid;
  std::thread thread;
  int exitCode;
};
#endif

AsyncProcess::AsyncProcess(AsyncProcess &&other) noexcept {
  m_implData = std::move(other.m_implData);
}

AsyncProcess &AsyncProcess::operator=(AsyncProcess &&other) noexcept {
  m_implData = std::move(other.m_implData);
  return *this;
}

bool AsyncProcess::running() const noexcept {
  return m_implData.get() && m_status->thread.joinable();
}

int64 AsyncProcess::join() {
  if (!m_implData.get())
    return -1;
  if (m_status->thread.joinable()) {
    m_status->thread.join();
  }
  return m_status->exitCode;
}

void AsyncProcess::detach() {
  if (!m_implData.get())
    return;
  pl_assert(m_status->thread.joinable());
  m_status->thread.detach();
}

#ifdef _WIN32

struct DeleterImpl {
  void operator()(void *implData) const {
    if (!implData)
      return;
    delete static_cast<WindowsProcessData *>(implData);
  }
};

static void WaitForProcess(std::shared_ptr<void> procInfo) {
  WindowsProcessData *info = static_cast<WindowsProcessData *>(procInfo.get());
  WaitForSingleObject(info->info.hProcess, INFINITE);

  DWORD exitCode;
  if (GetExitCodeProcess(info->info.hProcess, &exitCode) &&
      exitCode != 0xFFFFFFFFu) {
    info->exitCode = (int64)exitCode;
  } else {
    info->exitCode = -1;
  }

  CloseHandle(info->info.hProcess);
  CloseHandle(info->info.hThread);
}

string AsyncProcess::quoteAndEscapeArg(const string &str) {
  // Windows command line parsing is hilariously stupid.
  // Escape characters only work if they are followed by
  // a double quote
  int numEscapes = 0;
  string result = "\"";
  for (char c : str) {
    switch (c) {
    case '\\':
      numEscapes++;
      break;
    case '"':
      while (numEscapes-- >= 0)
        result += '\\';
      [[fallthrough]];
    default:
      numEscapes = 0;
      break;
    }
    result += c;
  }
  while (numEscapes-- > 0)
    result += '\\';
  result += "\"";
  return result;
}

AsyncProcess::AsyncProcess(const string &rawCmd) {
  m_implData =
      std::shared_ptr<void>((void *)new WindowsProcessData(), DeleterImpl());

  ZeroMemory(&m_status->si, sizeof(_STARTUPINFOW));
  ZeroMemory(&m_status->info, sizeof(PROCESS_INFORMATION));
  m_status->si.cb = sizeof(_STARTUPINFOW);

  std::wstring lpCommandLine = Unicode::toUtf16(rawCmd);
  if (!CreateProcessW(nullptr, lpCommandLine.data(), nullptr, nullptr, false,
                      CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP, nullptr,
                      nullptr, &m_status->si, &m_status->info)) {
    m_implData.reset();
    throw ProcessLaunchException(rawCmd, std::vector<string>(),
                                 (int)GetLastError());
  }

  m_status->exitCode = -1;
  m_status->thread = std::thread(WaitForProcess, m_implData);
}

AsyncProcess::AsyncProcess(const string &process,
                           const std::vector<string> &args) {
  m_implData =
      std::shared_ptr<void>((void *)new WindowsProcessData(), DeleterImpl());

  ZeroMemory(&m_status->si, sizeof(_STARTUPINFOW));
  ZeroMemory(&m_status->info, sizeof(PROCESS_INFORMATION));
  m_status->si.cb = sizeof(_STARTUPINFOW);

  string cmd = quoteAndEscapeArg(process);
  for (const string &arg : args) {
    cmd += ' ';
    cmd += quoteAndEscapeArg(arg);
  }

  std::wstring lpCommandLine = Unicode::toUtf16(cmd);
  if (!CreateProcessW(nullptr, lpCommandLine.data(), nullptr, nullptr, false,
                      CREATE_NO_WINDOW | CREATE_NEW_PROCESS_GROUP, nullptr,
                      nullptr, &m_status->si, &m_status->info)) {
    m_implData.reset();
    throw ProcessLaunchException(process, args, (int)GetLastError());
  }

  m_status->exitCode = -1;
  m_status->thread = std::thread(WaitForProcess, m_implData);
}

int64 AsyncProcess::pid() const noexcept {
  if (!m_implData.get() || !m_status->thread.joinable())
    return -1;
  return (int64)m_status->info.dwProcessId;
}

bool AsyncProcess::kill() {
  return m_implData.get() && m_status->thread.joinable() &&
         TerminateProcess(m_status->info.hProcess, 0xFFFFFFFFu);
}

static bool tryGetOutputHelper(const string &command, string &output,
                               bool useErrorOutput) {
  output.clear();

  _STARTUPINFOW si;
  PROCESS_INFORMATION pi;

  HANDLE stdOutRead = nullptr;
  HANDLE stdOutWrite = nullptr;

  SECURITY_ATTRIBUTES security;
  security.nLength = sizeof(SECURITY_ATTRIBUTES);
  security.bInheritHandle = true;
  security.lpSecurityDescriptor = nullptr;

  if (!CreatePipe(&stdOutRead, &stdOutWrite, &security, 1024)) {
    return false;
  }

  if (!SetHandleInformation(stdOutRead, HANDLE_FLAG_INHERIT, 0)) {
    return false;
  }

  ZeroMemory(&si, sizeof(si));
  ZeroMemory(&pi, sizeof(pi));
  si.cb = sizeof(si);
  if (useErrorOutput) {
    si.hStdError = stdOutWrite;
  } else {
    si.hStdOutput = stdOutWrite;
  }
  si.dwFlags |= STARTF_USESTDHANDLES;

  std::wstring lpCommandLine = Unicode::toUtf16(command);
  if (!CreateProcessW(nullptr, lpCommandLine.data(), nullptr, nullptr, true,
                      CREATE_NO_WINDOW, nullptr, nullptr, &si, &pi)) {
    return false;
  }

  if (!CloseHandle(stdOutWrite)) {
    return false;
  }

  DWORD bytesRead = 0;
  char buffer[1024];

  while (ReadFile(stdOutRead, buffer, 1023, &bytesRead, nullptr) &&
         bytesRead > 0) {
    buffer[bytesRead] = '\0';
    output += buffer;
  }

  if (!CloseHandle(stdOutRead)) {
    return false;
  }

  DWORD exitCode;
  if (!GetExitCodeProcess(pi.hProcess, &exitCode)) {
    return false;
  }

  if (!CloseHandle(pi.hProcess) || !CloseHandle(pi.hThread)) {
    return false;
  }

  return exitCode == 0;
}

bool Process::tryGetOutput(const string &command, string &output) {
  return tryGetOutputHelper(command, output, false);
}

bool Process::tryGetErrorOutput(const string &command, string &output) {
  return tryGetOutputHelper(command, output, true);
}

#else

struct DeleterImpl {
  void operator()(void *implData) const {
    if (!implData)
      return;
    delete static_cast<PosixProcessData *>(implData);
  }
};

static void awaitProcess(std::shared_ptr<void> procInfo) {
  PosixProcessData *status = static_cast<PosixProcessData *>(procInfo.get());
  waitpid(status->pid, &status->exitCode, 0);
}

AsyncProcess::AsyncProcess(const string &process,
                           const std::vector<string> &args) {
  const char *argv[args.size() + 2];
  argv[0] = process.c_str();
  for (size_t i = 0; i < args.size(); i++) {
    argv[i + 1] = args[i].c_str();
  }
  argv[args.size() + 1] = nullptr;

  posix_spawnattr_t spawnAttr;
  posix_spawnattr_init(&spawnAttr);

  m_implData =
      std::shared_ptr<void>((void *)new PosixProcessData(), DeleterImpl());
  int errorCode =
      posix_spawnp(&m_status->pid, process.c_str(), nullptr, &spawnAttr,
                   const_cast<char *const *>(argv), environ);

  if (errorCode != 0 || m_status->pid < 0) {
    m_implData.reset();
    throw ProcessLaunchException(process, args, errorCode);
  }

  m_status->exitCode = -1;
  m_status->thread = std::thread(awaitProcess, m_implData);
}

int64 AsyncProcess::pid() const noexcept {
  if (!m_implData.get() || !m_status->thread.joinable())
    return -1;
  return (int64)m_status->pid;
}

bool AsyncProcess::kill() {
  if (!m_implData.get() || !m_status->thread.joinable())
    return false;
  pl_assert(m_status->pid > 1);
  return std::system(("kill -9 "s + Number::toString(m_status->pid)).c_str()) ==
         0;
}

bool Process::tryGetOutput(const string &command, string &output) {
  output.clear();

  FILE *proc = popen(command.c_str(), "r");
  if (proc == nullptr)
    return false;

  char buffer[1024];
  while (fgets(buffer, 1024, proc) != nullptr) {
    output += buffer;
  }

  return pclose(proc) == 0;
}

bool Process::tryGetErrorOutput(const string &command, string &output) {
  return Process::tryGetOutput(command + " 2>&1 >/dev/null", output);
}

#endif

#undef m_status
