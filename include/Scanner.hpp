#pragma once

#include "ThreadPool.hpp"
#include "utils.hpp"

class Scanner {
public:
  explicit Scanner(const std::unordered_map<MD5Hash, std::string> &db);

  void scan(const fs::path &target);

  std::string get_scan_result();

private:
  static constexpr uint8_t MAX_THREADS = 8;
  std::unordered_map<MD5Hash, std::string> _db;
  ThreadPool _threadpool;
  size_t _processed_files = 0;
  size_t _bad_files = 0;
  size_t _malware_files = 0;
  void process_file(const fs::path &path);
};
