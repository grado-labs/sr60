#include <atomic>
#include <csignal>
#include <print>
#include <vector>

#include "bounded.hpp"
#include "nlohmann/json.hpp"
#include "sucker.hpp"
#include "utils.hpp"
#include "window.hpp"

using namespace std::chrono_literals;
using nlohmann::json;

std::atomic<bool> stop_request{false};

void signal_handler(int) {
  // Signal-safe: only set a lock-free atomic flag
  stop_request.store(true);
}

int main(int argc, char* argv[]) {
  // Correct termination (Ctrl+C)
  std::signal(SIGINT, signal_handler);

  // Log file with JSON Lines
  std::string logfile{"events.log"};
  if (argc >= 2) {
    logfile = argv[1];
  }

  // Obtain file
  std::error_code ec;
  const auto _ = std::filesystem::file_size(logfile, ec);
  if (ec) {
    std::print(stderr, "Cannot get file size: {}\n", ec.message());
    return 1;
  }

  // Thread-Safe File Line Reader
  sucker::safe_line_reader reader(logfile);

  // Thread-Safe Queue
  constexpr auto queue_capacity = 1000;
  bounded::safe_priority_queue<std::string, uint32_t> queue(queue_capacity);
  bounded::safe_priority_queue<std::string, uint32_t> capacitor(100);

  /************************************
   *       N-threaded file read       *
   ************************************/
  constexpr auto k_producers = 3;
  std::vector<std::thread> producers;

  for (auto k = 0; k < k_producers; ++k) {
    producers.emplace_back([&] {
      std::string line;

      while (reader.next_line(line)) {
        if (stop_request) {
          return;
        }

        if (line.length() == 0) continue;
        json j = json::parse(line);

        if (not queue.push(j["ts"], k, std::move(line))) {
          return;
        }
      }
    });
  }

  /*************************************
   *       M-threaded apply rule       *
   *************************************/
  constexpr auto k_consumers = 2;
  std::vector<std::thread> consumers;

  for (auto k = 0; k < k_consumers; ++k) {
    consumers.emplace_back([&] {
      while (auto line = queue.pop()) {
        json j = json::parse(*line);

        if (not capacitor.push(j["ts"], k, std::move(*line))) {
          return;
        }
      }
    });
  }

  // Sliding window
  window::sliding_ar<uint32_t> ar(10s);

  // Arbitrage
  std::thread arbitrator([&] {
    size_t last_size = 0;

    while (true) {
      // accumulate enough
      if (capacitor.size() < 50) {
        std::this_thread::sleep_for(20ms);

        // if no recent activity
        if (capacitor.size() == last_size) {
          std::this_thread::sleep_for(200ms);

          // last try to keep alive
          if (capacitor.size() == last_size) {
            capacitor.close();
            return;
          }
        }
      }

      auto line = capacitor.pop();
      if (not line) {
        return;
      }

      json j = json::parse(*line);

      // Apply detection rule for ultra-fertile processes
      if (j["type"] == "ProcessStarted") {
        ar.add(utils::unix2tp(j["ts"]), j["ppid"], j["pid"]);
      }
    }
  });

  for (auto& producer : producers) {
    producer.join();
  }

  queue.close();

  for (auto& consumer : consumers) {
    consumer.join();
  }

  arbitrator.join();
}
