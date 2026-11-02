#include <string>

#include "ice.hpp"

int main(int argc, char* argv[]) {
  // default arg values
  std::string dirpath{"."};
  std::string jsonman{"baseline.json"};

  // presented arg values
  if (argc >= 3) {
    dirpath = argv[1];
    jsonman = argv[2];
  }

  auto res = ice::run(dirpath, jsonman);
  return res;
}
