#pragma once

#include <charconv>
#include <chrono>

namespace utils {

using std::chrono::steady_clock;

steady_clock::time_point unix2tp(uint32_t unix_seconds) {
  return steady_clock::time_point{std::chrono::seconds{unix_seconds}};
}

template <typename T> bool vtonum(const std::string_view& view, T& value) {
  if (view.empty()) {
    return false;
  }

  const char* first = view.data();
  const char* last = view.data() + view.length();

  std::from_chars_result res = std::from_chars(first, last, value);

  if (res.ec != std::errc()) {
    return false;
  }

  if (res.ptr != last) {
    return false;
  }

  return true;
}

}  // namespace utils
