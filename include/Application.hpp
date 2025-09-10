#pragma once

#include "utils.hpp"

class Application {
public:
  static int run(const fs::path &database, const fs::path &log,
                 const fs::path &target);
};
