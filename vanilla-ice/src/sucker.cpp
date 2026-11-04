#include "sucker.hpp"

#include <chrono>
#include <thread>

namespace sucker {

using namespace std::chrono_literals;

safe_line_reader::safe_line_reader(std::string path) : file_(std::move(path)) {}

bool safe_line_reader::next_line(std::string& line) {
  {
    std::lock_guard lock(mutex_);

    if (not std::getline(file_, line)) {
      return false;
    }
  }

  // sleep as required in task:
  //   1ms is enough to make a mess
  std::this_thread::sleep_for(1ms);

  return true;
}

}  // namespace sucker
