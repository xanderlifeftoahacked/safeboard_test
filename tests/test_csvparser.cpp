#include "CSVParser.hpp"
#include <filesystem>
#include <fstream>
#include <gtest/gtest.h>

class CSVParserTest : public ::testing::Test {
protected:
  std::string testFile = "test.csv";

  void TearDown() override { std::filesystem::remove(testFile); }
};

TEST_F(CSVParserTest, LoadsValidCSV) {
  {
    std::ofstream out(testFile);
    out << "hash1;clean\n";
    out << "hash2;infected\n";
  }

  CSVParser parser;
  EXPECT_TRUE(parser.load(testFile));

  auto db = parser.get_db();
  EXPECT_EQ(db.at("hash1"), "clean");
  EXPECT_EQ(db.at("hash2"), "infected");
}

TEST_F(CSVParserTest, FailsOnBadCSV1) {
  {
    std::ofstream out(testFile);
    out << "onlyhash_without_verdict;\n";
  }

  CSVParser parser;
  EXPECT_FALSE(parser.load(testFile));
}

TEST_F(CSVParserTest, FailsOnBadCSV2) {
  {
    std::ofstream out(testFile);
    out << ";\n";
  }

  CSVParser parser;
  EXPECT_FALSE(parser.load(testFile));
}

TEST_F(CSVParserTest, FailsOnBadCSV3) {
  {
    std::ofstream out(testFile);
    out << "a;b\n";
    out << "c;d \n";
    out << "one;two;three\n";
  }

  CSVParser parser;
  EXPECT_FALSE(parser.load(testFile));
}
