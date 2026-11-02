#include <filesystem>
#include <fstream>
#include <list>
#include <print>
#include <tuple>

#include "nlohmann/json.hpp"
using nlohmann::json;

#include "json-line.hpp"
#include "sha256.hpp"

namespace ice {

namespace fs = std::filesystem;

int run(const std::string& dirpath, const std::string& jsonman) {
  auto absdir = fs::absolute(dirpath);

  // auxiliary lists
  std::list<std::string> new_files{};
  std::list<std::string> changed_files{};
  std::list<std::string> deleted_files{};

  // read + parse from input file
  std::ifstream ifs{jsonman};
  json jd = json::parse(ifs);

  // baseline as hash map
  std::unordered_map<std::string, std::tuple<std::string, bool>> baseline{};
  for (const auto& [filepath, hashsum] : jd.items()) {
    auto abspath{absdir};
    abspath /= fs::path(filepath);

    baseline.insert({abspath, {hashsum, false}});
  }

  // actual fs traverse from given dir + lookup in baseline
  for (const auto& entry : fs::recursive_directory_iterator(absdir)) {
    std::string filepath{entry.path()};

    // regular file only
    if (entry.is_regular_file()) {
      auto it = baseline.find(filepath);

      if (it != baseline.end()) {  // in baseline
        auto& [hashsum, lookup] = baseline[filepath];
        lookup = true;

        if (sha256::sum(filepath) != hashsum) {
          changed_files.push_back(filepath);
          std::print(stdout, "{}", json_line::event("file_modified", filepath));
        }
      } else {  // out of baseline
        new_files.push_back(filepath);
        std::print(stdout, "{}", json_line::event("file_added", filepath));
      }
    }
  }

  // get the orphans done
  for (const auto& entry : baseline) {
    const auto& filepath = entry.first;
    const auto& [hashsum, lookup] = entry.second;

    if (not lookup) {
      deleted_files.push_back(filepath);
      std::print(stdout, "{}", json_line::event("file_removed", filepath));
    }
  }

  return 0;
}
}  // namespace ice
