#pragma once

#include <fstream>
#include <mutex>
#include <string>

#include "bounded.hpp"

namespace sucker {

class safe_line_reader {
 public:
  explicit safe_line_reader(std::string path);

 public:
  bool next_line(std::string& line);

 private:
  std::ifstream file_;
  std::mutex mutex_;
};

}  // namespace sucker
