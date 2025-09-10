#include <gtest/gtest.h>
#include <regex>

#include "Logger.hpp"
#include "Scanner.hpp"
#include "utils.hpp"

namespace fs = std::filesystem;

static void write_file(const fs::path &p, std::string_view content) {
  fs::create_directories(p.parent_path());
  std::ofstream out(p, std::ios::binary);
  ASSERT_TRUE(out.is_open()) << "Failed to create file: " << p.string();
  out.write(content.data(), static_cast<std::streamsize>(content.size()));
}

static std::vector<std::string> read_all_lines(const fs::path &p) {
  std::ifstream in(p);
  std::vector<std::string> lines;
  std::string line;
  while (std::getline(in, line))
    lines.push_back(line);
  return lines;
}

class ScannerTest : public ::testing::Test {
protected:
  fs::path tmpDir = fs::temp_directory_path() / "scanner_tests_tmp";
  fs::path logFile = tmpDir / "report.log";
  fs::path scanRoot = tmpDir / "scan_root";

  void SetUp() override {
    std::error_code ec;
    fs::remove_all(tmpDir, ec);
    fs::create_directories(scanRoot, ec);
    Logger::get_instance().shutdown();
  }

  void TearDown() override {
    Logger::get_instance().shutdown();
    std::error_code ec;
    fs::remove_all(tmpDir, ec);
  }

  static void extract_counts(const std::string &s, int &total, int &malware,
                             int &bad) {
    std::regex re_total(R"(Total files:?\s*(\d+))");
    std::regex re_mal(R"(Malware files:\s*(\d+))");
    std::regex re_bad(R"(Bad files.*:\s*(\d+))");

    auto get_num = [&](const std::regex &re) -> int {
      std::smatch m;
      if (std::regex_search(s, m, re) && m.size() >= 2)
        return std::stoi(m[1]);
      return -1;
    };

    total = get_num(re_total);
    malware = get_num(re_mal);
    bad = get_num(re_bad);
  }
};

TEST_F(ScannerTest, CommonScan) {
  auto goodFile = scanRoot / "good.txt";
  auto badFile = scanRoot / "subdir" / "evil.bin";
  write_file(goodFile, "just some safe text");
  write_file(badFile, "hello");

  std::unordered_map<MD5Hash, std::string> db;
  db[utils::calculate_md5(badFile)] = "infected";

  ASSERT_TRUE(Logger::get_instance().init(logFile.string()));

  Scanner scanner(db);
  scanner.scan(scanRoot);
  Logger::get_instance().shutdown();

  auto lines = read_all_lines(logFile);
  ASSERT_FALSE(lines.empty());
  EXPECT_EQ(lines[0], "file_path;hash;verdict");
  ASSERT_EQ(lines.size(), 2u);

  int total = -1, malware = -1, bad = -1;
  auto summary = scanner.get_scan_result();
  extract_counts(summary, total, malware, bad);

  EXPECT_EQ(total, 2);
  EXPECT_EQ(malware, 1);
  EXPECT_EQ(bad, 0);
}

TEST_F(ScannerTest, NestedFiles) {
  auto f1 = scanRoot / "a" / "b" / "c" / "x.dat";
  auto f2 = scanRoot / "a" / "y.txt";
  write_file(f1, "hello");
  write_file(f2, "something");

  std::unordered_map<MD5Hash, std::string> db;
  db[utils::calculate_md5(f1)] = "infected";

  ASSERT_TRUE(Logger::get_instance().init(logFile.string()));

  Scanner scanner(db);
  scanner.scan(scanRoot);
  Logger::get_instance().shutdown();

  auto lines = read_all_lines(logFile);
  ASSERT_EQ(lines.size(), 2u);

  int total = -1, malware = -1, bad = -1;
  extract_counts(scanner.get_scan_result(), total, malware, bad);
  EXPECT_EQ(total, 2);
  EXPECT_EQ(malware, 1);
  EXPECT_EQ(bad, 0);
}

#ifdef __unix__
#include <sys/stat.h>
TEST_F(ScannerTest, UnaccessableFile) {
  auto unread = scanRoot / "no_read.txt";
  write_file(unread, "secret");
  ::chmod(unread.c_str(), 0000);

  std::unordered_map<MD5Hash, std::string> db;
  ASSERT_TRUE(Logger::get_instance().init(logFile.string()));

  Scanner scanner(db);
  scanner.scan(scanRoot);

  int total = -1, malware = -1, bad = -1;
  extract_counts(scanner.get_scan_result(), total, malware, bad);

  EXPECT_GE(total, 1);
  EXPECT_EQ(malware, 0);
  EXPECT_GE(bad, 1);
}
#endif
