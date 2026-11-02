#include "json-line.hpp"

#include <sstream>

namespace json_line {

std::string event(const std::string& op, const std::string& path) {
  std::ostringstream oss;

  oss << R"({"op":")" << op << R"(","path":")" << path << R"("})" << '\n';
  return oss.str();
}

}  // namespace json_line
