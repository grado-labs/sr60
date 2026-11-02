#pragma once

#include <condition_variable>
#include <cstddef>
#include <deque>
#include <mutex>
#include <optional>
#include <queue>
#include <utility>

namespace bounded {

template <typename T, typename Priority = uint32_t> class safe_priority_queue {
 public:
  explicit safe_priority_queue(std::size_t capacity) : capacity_(capacity) {}

  // lower Priority value == higher priority
  bool push(Priority priority, std::uint64_t sequence, T value) {
    std::unique_lock lock(mutex_);

    not_full_.wait(lock, [&] { return closed_ or queue_.size() < capacity_; });

    if (closed_) {
      return false;
    }

    queue_.emplace(priority, sequence, std::move(value));
    not_empty_.notify_one();

    return true;
  }

  std::optional<T> top() {
    std::unique_lock lock(mutex_);

    not_empty_.wait(lock, [&] { return !queue_.empty() || closed_; });

    if (queue_.empty()) {
      return std::nullopt;
    }

    T value = const_cast<T&>(queue_.top().value);

    not_empty_.notify_one();
    return value;
  }

  std::optional<T> pop() {
    std::unique_lock lock(mutex_);

    not_empty_.wait(lock, [&] { return !queue_.empty() || closed_; });

    if (queue_.empty()) {
      return std::nullopt;
    }

    T value = std::move(const_cast<T&>(queue_.top().value));
    queue_.pop();

    not_full_.notify_one();
    return value;
  }

  void close() {
    {
      std::lock_guard lock(mutex_);
      closed_ = true;
    }
    not_empty_.notify_all();
    not_full_.notify_all();
  }

  std::size_t size() const {
    std::lock_guard lock(mutex_);
    return queue_.size();
  }

 private:
  struct item {
    Priority priority;
    std::uint64_t sequence;
    T value;

    // min-heap: smallest priority first; sequence breaks ties
    bool operator<(const item& other) const {
      return std::tie(priority, sequence) >
             std::tie(other.priority, other.sequence);
    }
  };

  std::size_t capacity_;

  std::priority_queue<item> queue_;
  mutable std::mutex mutex_;

  std::condition_variable not_empty_;
  std::condition_variable not_full_;
  bool closed_ = false;
};

}  // namespace bounded