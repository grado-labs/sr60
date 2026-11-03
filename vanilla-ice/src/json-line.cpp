#include "json-line.hpp"

#include <sstream>

namespace json_line {

std::string event(const std::string& op, const std::string& path) {
  std::ostringstream oss;

  oss << R"({"op":")" << op << R"(","path":")" << path << R"("})" << '\n';
  return oss.str();
}

std::string activity_alert(uint32_t ppid, const std::list<uint32_t>& pids) {
  std::ostringstream oss;

  oss << R"({"alert":")" << "SuspiciousActivityAlert" << R"(","ppid":)" << ppid
      << R"(","pids":[)";

  for (const auto pid : pids) {
    oss << pid << ",";
  }

  oss.seekp(-1, std::ios_base::end);
  oss << R"(]})" << '\n';

  return oss.str();
}

}  // namespace json_line
