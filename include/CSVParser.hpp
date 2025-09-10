#pragma once

#include "utils.hpp"
#include <unordered_map>

class CSVParser {
public:
  static constexpr int32_t MAX_DB_SIZE = 500 * 1024 * 1024;
  bool load(const fs::path &path);
  const std::unordered_map<MD5Hash, std::string> &get_db() const;

private:
  std::unordered_map<MD5Hash, std::string> _database;
  bool validate_file(const fs::path &path);
};
