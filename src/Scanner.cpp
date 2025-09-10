#include "Scanner.hpp"
#include "Logger.hpp"

Scanner::Scanner(const std::unordered_map<MD5Hash, std::string> &db)
    : _db{db}, _threadpool{ThreadPool(
                   std::min(std::thread::hardware_concurrency(),
                            static_cast<unsigned int>(MAX_THREADS)))} {}

void Scanner::process_file(const fs::path &path) {
  MD5Hash hash = utils::calculate_md5(path);

  ++_processed_files;
  if (hash.empty()) {
    ++_bad_files;
    std::cerr << "Cant open: " << path << '\n';
    return;
  }

  if (!_db.contains(hash))
    return;

  ++_malware_files;
  std::string verdict = _db[hash];
  Logger::get_instance().log(path, hash, verdict);
}

void Scanner::scan(const fs::path &target) {
  std::error_code ec;
  for (const auto &file : fs::recursive_directory_iterator(
           target, fs::directory_options::skip_permission_denied)) {
    if (!file.is_regular_file())
      continue;

    if (ec) {
      ++_bad_files, ++_processed_files;
      ec.clear();
      std::cerr << "Error: " << ec.message() << '\n';
      continue;
    }

    _threadpool.enqueue([this, file]() { process_file(file.path()); });
  }

  _threadpool.wait();
  _threadpool.stop();
};

std::string Scanner::get_scan_result() {
  std::stringstream ss;
  ss << "Result: \n"
     << "Total files:" << _processed_files << '\n'
     << "Malware files: " << _malware_files << '\n'
     << "Bad files (probably have no access): " << _bad_files;

  return ss.str();
}
