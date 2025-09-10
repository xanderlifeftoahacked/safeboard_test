
#include "utils.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

class UtilsTest : public ::testing::Test {
protected:
  std::string testFile = "test_md5.txt";

  void TearDown() override { std::filesystem::remove(testFile); }
};

TEST_F(UtilsTest, Trim_Common) {
  EXPECT_EQ(utils::trim("   hello   "), "hello");
  EXPECT_EQ(utils::trim("\t\n  world\t\n "), "world");
}

TEST_F(UtilsTest, Trim_Empty) {
  EXPECT_EQ(utils::trim(""), "");
  EXPECT_EQ(utils::trim("     "), "");
}

TEST_F(UtilsTest, Calc_MD5_1) {
  {
    std::ofstream out(testFile);
  }
  std::string md5 = utils::calculate_md5(testFile);
  EXPECT_EQ(md5, "d41d8cd98f00b204e9800998ecf8427e");
}

TEST_F(UtilsTest, Calc_MD5_2) {
  {
    std::ofstream out(testFile);
    out << "check";
  }
  std::string md5 = utils::calculate_md5(testFile);
  EXPECT_EQ(md5, "0ba4439ee9a46d9d9f14c60f88f45f87");
}
