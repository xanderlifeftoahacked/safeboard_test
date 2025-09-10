#include "Logger.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>
#include <string>
#include <thread>
#include <vector>

class LoggerTest : public ::testing::Test {
protected:
  std::string logFile = "test_log.csv";

  void TearDown() override { std::filesystem::remove(logFile); }

  std::vector<std::string> readAllLines() {
    std::ifstream in(logFile);
    std::vector<std::string> lines;
    std::string line;
    while (std::getline(in, line)) {
      lines.push_back(line);
    }
    return lines;
  }
};

TEST_F(LoggerTest, InitLog) {
  auto &logger = Logger::get_instance();
  EXPECT_TRUE(logger.init(logFile));

  auto lines = readAllLines();
  EXPECT_FALSE(lines.empty());
  EXPECT_EQ(lines[0], "file_path;hash;verdict");
  logger.shutdown();
}

TEST_F(LoggerTest, LogWrite) {
  auto &logger = Logger::get_instance();
  EXPECT_TRUE(logger.init(logFile));

  logger.log("file1.txt", "hash1", "clean");
  logger.log("file2.txt", "hash2", "infected");
  logger.shutdown();

  auto lines = readAllLines();
  EXPECT_GE(lines.size(), 3u);
  EXPECT_EQ(lines[1], "file1.txt;hash1;clean");
  EXPECT_EQ(lines[2], "file2.txt;hash2;infected");
}

TEST_F(LoggerTest, MultipleInit) {
  auto &logger = Logger::get_instance();
  EXPECT_TRUE(logger.init(logFile));
  logger.shutdown();
  EXPECT_TRUE(logger.init(logFile));
  logger.shutdown();

  auto lines = readAllLines();
  EXPECT_EQ(lines.size(), 1u);
  EXPECT_EQ(lines[0], "file_path;hash;verdict");
}

TEST_F(LoggerTest, Shutdown) {
  auto &logger = Logger::get_instance();
  EXPECT_TRUE(logger.init(logFile));
  logger.shutdown();

  std::ofstream out(logFile, std::ios::app);
  EXPECT_TRUE(out.is_open());
}

TEST_F(LoggerTest, MultiThreadWrite) {
  auto &logger = Logger::get_instance();
  EXPECT_TRUE(logger.init(logFile));

  const int num_threads = 8;
  const int logs_per_thread = 50;

  std::vector<std::thread> threads;
  for (int t = 0; t < num_threads; ++t) {
    threads.emplace_back([t]() {
      for (int i = 0; i < logs_per_thread; ++i) {
        Logger::get_instance().log(
            "file" + std::to_string(t) + "_" + std::to_string(i) + ".txt",
            "hash" + std::to_string(t) + "_" + std::to_string(i),
            (i % 2 == 0) ? "clean" : "infected");
      }
    });
  }

  for (auto &th : threads)
    th.join();
  logger.shutdown();

  auto lines = readAllLines();

  EXPECT_EQ(lines.size(), 1 + num_threads * logs_per_thread);
  EXPECT_EQ(lines[0], "file_path;hash;verdict");

  EXPECT_NE(std::find(lines.begin(), lines.end(), "file0_0.txt;hash0_0;clean"),
            lines.end());
  EXPECT_NE(
      std::find(lines.begin(), lines.end(), "file7_49.txt;hash7_49;infected"),
      lines.end());
}
