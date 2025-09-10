#include "Application.hpp"

int main(int argc, char **argv) {
  utils::Args args = utils::parse_args(argc, argv);

  if (args.bad)
    return 1;

  return Application::run(args.base, args.log, args.path);
}
