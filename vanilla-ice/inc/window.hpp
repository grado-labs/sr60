#pragma once

#include <chrono>
#include <deque>
#include <list>
#include <map>
#include <mutex>
#include <utility>

#include "json-line.hpp"

namespace window {

using std::deque;
using std::get;
using std::list;
using std::lock_guard;
using std::map;
using std::mutex;
using std::print;
using std::tuple;

// Window sliding to apply rule
template <typename T, typename Clock = std::chrono::steady_clock>
class sliding_ar {
 public:
  using TimePoint = typename Clock::time_point;
  using Duration = typename Clock::duration;

  explicit sliding_ar(Duration window) : window_(window) {}

  void add(TimePoint timestamp, T parent, T child) {
    lock_guard lock(mutex_);

    expel(timestamp);
    events_.emplace_back(timestamp, parent, child);

    map<uint32_t, tuple<uint32_t, list<uint32_t>>> ppid2count;

    for (const auto& [timestamp, ppid, pid] : events_) {
      auto& k = get<0>(ppid2count[ppid]);
      auto& pids = get<1>(ppid2count[ppid]);

      ++k;
      pids.push_back(pid);

      // Detection rule
      if (k > 5) {
        print(stdout, "{}", json_line::activity_alert(ppid, pids));
      }
    }
  }

 private:
  void expel(TimePoint now) {
    const auto cutoff = now - window_;

    while (not events_.empty() and get<0>(events_.front()) < cutoff) {
      events_.pop_front();
    }
  }

 private:
  Duration window_;
  mutable mutex mutex_;

  // deque: {timestamp, ppid, pid}
  deque<tuple<TimePoint, T, T>> events_;
};

}  // namespace window
