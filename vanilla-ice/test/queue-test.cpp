#include <atomic>
#include <chrono>
#include <future>
#include <thread>
#include <vector>

#include "bounded.hpp"
#include "gtest/gtest.h"

namespace vanilla_ice {

using namespace std::chrono_literals;
using Q = bounded::safe_priority_queue<std::string, int>;

TEST(SafePriorityQueueTest, PopFromEmptyReturnsNulloptAfterClose) {
  Q queue(2);

  queue.close();

  EXPECT_FALSE(queue.pop().has_value());
}

TEST(SafePriorityQueueTest, PushBlocksWhenFullThenSucceedsAfterPop) {
  Q queue(1);

  ASSERT_TRUE(queue.push(1, 1, "first"));

  auto producer = std::async(std::launch::async,
                             [&] { return queue.push(2, 2, "second"); });

  // Give the producer time to block on the full queue
  std::this_thread::sleep_for(50ms);

  EXPECT_FALSE(producer.wait_for(0ms) == std::future_status::ready);

  ASSERT_TRUE(queue.pop().has_value());

  EXPECT_TRUE(producer.get());
  EXPECT_EQ(queue.size(), 1);

  queue.close();
}

TEST(SafePriorityQueueTest, CloseRejectsNewPushes) {
  Q queue(2);

  queue.close();

  EXPECT_FALSE(queue.push(1, 1, "ignored"));
  EXPECT_FALSE(queue.pop().has_value());

  queue.close();
}

TEST(SafePriorityQueueTest, ConsumersDrainItemsAfterClose) {
  Q queue(10);

  ASSERT_TRUE(queue.push(1, 1, "one"));
  ASSERT_TRUE(queue.push(2, 2, "two"));

  queue.close();

  EXPECT_EQ(queue.pop(), "one");
  EXPECT_EQ(queue.pop(), "two");
  EXPECT_FALSE(queue.pop().has_value());

  queue.close();
}

TEST(SafePriorityQueueTest, MultipleProducersAndConsumers) {
  constexpr int k_producers = 2;
  constexpr int k_consumers = 3;
  constexpr int k_items_per_producer = 1000;

  Q queue(100);

  std::atomic<int> produced{0};
  std::atomic<int> consumed{0};

  std::vector<std::thread> producers;
  for (int p = 0; p < k_producers; ++p) {
    producers.emplace_back([&, p] {
      for (int i = 0; i < k_items_per_producer; ++i) {
        const int priority = (p + i) % 10;
        const std::uint64_t sequence =
            static_cast<std::uint64_t>(p) * k_items_per_producer + i;

        if (queue.push(priority, sequence, "item")) ++produced;
      }
    });
  }

  std::vector<std::thread> consumers;
  for (int c = 0; c < k_consumers; ++c) {
    consumers.emplace_back([&] {
      while (auto item = queue.pop()) {
        EXPECT_FALSE(item->empty());
        ++consumed;
      }
    });
  }

  for (auto& producer : producers) producer.join();

  queue.close();

  for (auto& consumer : consumers) consumer.join();

  EXPECT_EQ(produced.load(), k_producers * k_items_per_producer);
  EXPECT_EQ(consumed.load(), k_producers * k_items_per_producer);
  EXPECT_EQ(queue.size(), 0);
}

TEST(SafePriorityQueueTest, PopBlocksUntilProducerPushes) {
  Q queue(1);

  auto consumer = std::async(std::launch::async, [&] { return queue.pop(); });

  std::this_thread::sleep_for(50ms);

  EXPECT_FALSE(consumer.wait_for(0ms) == std::future_status::ready);

  ASSERT_TRUE(queue.push(1, 1, "value"));

  auto result = consumer.get();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(*result, "value");
}
}  // namespace vanilla_ice