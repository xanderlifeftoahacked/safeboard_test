#pragma once

#include "utils.hpp"
#include <atomic>
#include <mutex>

class Logger {
public:
  Logger(const Logger &) = delete;
  Logger &operator=(const Logger &) = delete;

  static Logger &get_instance();

  bool init(const fs::path &log_path);

  void log(const fs::path &file_path, const std::string &hash,
           const std::string &verdict);

  void shutdown();

private:
  Logger();
  ~Logger();

  std::ofstream _log_file;
  std::mutex _log_mtx;
  std::atomic<bool> _inited;
};
