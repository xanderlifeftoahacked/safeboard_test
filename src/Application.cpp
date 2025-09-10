#include "Application.hpp"
#include "CSVParser.hpp"
#include "Logger.hpp"
#include "Scanner.hpp"
#include <chrono>
#include <filesystem>
#include <iostream>

int Application::run(const fs::path &database, const fs::path &log,
                     const fs::path &target) {

  CSVParser parser;

  if (!parser.load(database))
    return 1;
  if (!Logger::get_instance().init(log))
    return 1;

  Scanner scanner(parser.get_db());
  std::cout << "Starting scanning..." << std::endl;

  auto start = std::chrono::system_clock::now();

  scanner.scan(target);
  Logger::get_instance().shutdown();

  auto end = std::chrono::system_clock::now();

  std::cout << scanner.get_scan_result() << std::endl;

  std::cout << std::chrono::duration_cast<std::chrono::seconds>(end - start)
            << " elapsed" << std::endl;
  ;

  return 0;
}
