#include "Logger.hpp"

Logger::Logger() : _inited{false} {}

Logger::~Logger() { shutdown(); }

Logger &Logger::get_instance() {
  static Logger logger;
  return logger;
}

bool Logger::init(const fs::path &log_path) {
  std::lock_guard<std::mutex> lock(_log_mtx);

  if (_inited)
    return true;

  _log_file.open(log_path, std::ios::out | std::ios::app);

  if (!_log_file.is_open()) {
    std::cerr << "Failed to open log file: " << log_path << '\n';
    return false;
  }

  _log_file.seekp(0, std::ios::end);
  if (_log_file.tellp() == 0) {
    _log_file << "file_path;hash;verdict" << std::endl;
  }

  _inited = true;
  return true;
}

void Logger::log(const fs::path &file_path, const std::string &hash,
                 const std::string &verdict) {

  std::lock_guard<std::mutex> lock(_log_mtx);

  _log_file << file_path.string() << ';' << hash << ';' << verdict << '\n';
}

void Logger::shutdown() {
  std::lock_guard<std::mutex> lock(_log_mtx);

  if (_log_file.is_open()) {
    _log_file.flush();
    _log_file.close();
  }

  _inited = false;
}
