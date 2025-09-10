#include "CSVParser.hpp"
#include "utils.hpp"

bool CSVParser::validate_file(const fs::path &path) {
  if (!fs::exists(path)) {
    std::cerr << "Error: file doesnt exist: " << path << '\n';
    return false;
  }

  auto size = fs::file_size(path);
  if (size > MAX_DB_SIZE) {
    std::cerr << "Database is too large: " << size
              << " bytes. This very simple 'antivirus' "
              << "supprots up to " << MAX_DB_SIZE
              << " Bytes. Because stores DB in RAM\n";
    return false;
  }

  return true;
}

bool CSVParser::load(const fs::path &path) {
  if (!validate_file(path)) {
    return false;
  }

  std::ifstream file(path);
  if (!file) {
    std::cerr << "Error: Cant open file: " << path << '\n';
    return false;
  }

  std::string line;
  size_t line_num = 0;

  while (std::getline(file, line)) {
    ++line_num;
    if (line.empty())
      continue;

    line = utils::trim(line);
    std::stringstream ss(line);

    MD5Hash hash = {};
    std::string verdict = {};

    if (!std::getline(ss, hash, ';') || !std::getline(ss, verdict, ';') ||
        !ss.eof()) {
      std::cerr << "Bad line: " << path << ':' << line_num << '\n';
      return false;
    }

    _database[utils::trim(hash)] = utils::trim(verdict);
  }

  return true;
}

const std::unordered_map<MD5Hash, std::string> &CSVParser::get_db() const {
  return _database;
}
