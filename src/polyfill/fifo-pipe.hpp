#ifndef SRC_POLYFILL_IS_VIEWER_HPP_
#define SRC_POLYFILL_IS_VIEWER_HPP_

#include "src/types.hpp"
#include <system_error>

class ReadableFifoPipe {

private:
  string m_filePath;
  bool m_alive;
#ifdef _WIN32
  void *m_handle;
#else
  int m_fd;
#endif

public:
  ReadableFifoPipe() noexcept;
  inline ~ReadableFifoPipe() noexcept { close(); }

  bool open(const string &name, std::error_code &err);
  bool connect(std::error_code &err) const noexcept;
  bool close(std::error_code &err) noexcept;
  bool read(size_t bytes, void *buffer, std::error_code &err) const noexcept;

  inline void close() {
    std::error_code err;
    close(err);
  }

  inline bool active() const noexcept { return m_alive; }

  inline void cancel() noexcept { m_alive = false; }

  inline const string &filePath() const noexcept { return m_filePath; }
};

class WritableFifoPipe {

private:
  string m_filePath;
#ifdef _WIN32
  bool m_alive;
  void *m_handle;
#else
  bool m_cancelled;
  int m_fd;
  bool m_created;
#endif

public:
  WritableFifoPipe() noexcept;
  inline ~WritableFifoPipe() noexcept { close(); }

  bool open(const string &name, std::error_code &err);
  bool connect(std::error_code &err) noexcept;
  bool close(std::error_code &err) noexcept;
  bool write(size_t bytes, const void *buffer,
             std::error_code &err) const noexcept;

  inline void cancel() noexcept {
#ifdef _WIN32
    m_alive = false;
#else
    m_cancelled = true;
#endif
  }

  inline void close() {
    std::error_code err;
    close(err);
  }

  inline const string &filePath() const noexcept { return m_filePath; }
};

#endif
