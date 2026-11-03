#include "sucker.hpp"

#include <fstream>
#include <print>

#include "nlohmann/json.hpp"
using nlohmann::json;

namespace sucker {

void read_range(uint32_t producer_id, const std::filesystem::path& path,
                std::uintmax_t begin, std::uintmax_t end,
                bounded::safe_priority_queue<std::string, uint32_t>& queue) {
  std::ifstream file(path, std::ios::binary);
  if (!file) {
    std::print(stderr, "Cannot open `{}`\n", path.c_str());
    return;
  }

  file.seekg(static_cast<std::streamoff>(begin));
  if (!file) {
    std::print(stderr, "Cannot seek in `{}`\n", path.c_str());
    return;
  }

  if (begin != 0) {
    std::string partial;
    if (!std::getline(file, partial)) {
      return;
    };
  }

  std::string line;
  while (static_cast<std::uintmax_t>(file.tellg()) < end and
         std::getline(file, line)) {
    if (line.length() == 0) {
      continue;
    }

    json j = json::parse(line);
    if (!queue.push(j["ts"], producer_id, std::move(line))) {
      return;
    }
  }
}

}  // namespace sucker
