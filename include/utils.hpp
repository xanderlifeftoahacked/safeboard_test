#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <openssl/md5.h>
#include <sstream>

using MD5Hash = std::string;
namespace fs = std::filesystem;

namespace utils {
constexpr size_t BUFFER_SIZE = 16384;

struct Args {
  fs::path base, log, path;
  bool bad = false;
};

inline std::string trim(const std::string &str) {
  auto start = str.begin();
  auto end = str.end();

  while (start != end && std::isspace(static_cast<unsigned char>(*start)))
    ++start;

  while (end != start && std::isspace(static_cast<unsigned char>(*(end - 1))))
    --end;

  return {start, end};
}

inline std::string calculate_md5(const fs::path &filename) {
  std::ifstream file(filename, std::ios::binary);
  if (!file.is_open())
    return "";

  MD5_CTX md5Context;
  MD5_Init(&md5Context);

  std::array<char, BUFFER_SIZE> buffer;

  while (file.read(buffer.data(), buffer.size()) || file.gcount() > 0) {
    MD5_Update(&md5Context, buffer.data(), file.gcount());
  }

  std::array<unsigned char, MD5_DIGEST_LENGTH> digest;
  MD5_Final(digest.data(), &md5Context);

  std::ostringstream result;
  result << std::hex << std::setfill('0');
  for (auto byte : digest) {
    result << std::setw(2) << static_cast<int32_t>(byte);
  }

  return result.str();
}

inline Args parse_args(int argc, char **argv) {
  Args args;
  if (argc != 7) {
    std::cerr << "Usage: " << argv[0]
              << " --base <file.csv> --log <file.log> --path <dir>\n";
    args.bad = true;
  }

  for (int i = 1; i < argc; i += 2) {
    std::string key = argv[i];
    std::string val = argv[i + 1];

    if (key == "--base") {
      args.base = std::move(val);
      continue;
    }

    if (key == "--log") {
      args.log = std::move(val);
      continue;
    }

    if (key == "--path") {
      args.path = std::move(val);
      continue;
    }

    std::cerr << "Unknown option: " << key << '\n';
    args.bad = true;
    break;
  }

  if (args.base.empty() || args.log.empty() || args.path.empty()) {
    std::cerr << "Required: --base, --log, --path\n";
    args.bad = true;
  }

  return args;
}

} // namespace utils
