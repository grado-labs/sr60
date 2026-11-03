#include <print>
#include <vector>

#include "bounded.hpp"
#include "nlohmann/json.hpp"
#include "sucker.hpp"
#include "utils.hpp"
#include "window.hpp"

using namespace std::chrono_literals;
using nlohmann::json;

int main(int argc, char* argv[]) {
  std::string logfile{"events.log"};
  if (argc >= 2) {
    logfile = argv[1];
  }

  std::error_code ec;
  const auto file_size = std::filesystem::file_size(logfile, ec);
  if (ec) {
    std::print(stderr, "Cannot get file size: {}\n", ec.message());
    return 1;
  }

  constexpr const auto queue_capacity = 1000;
  bounded::safe_priority_queue<std::string, uint32_t> queue(queue_capacity);

  std::thread producer1(
      [&] { sucker::read_range(1, logfile, 0, file_size, queue); });

  /*************************************
   *       N-threaded apply rule       *
   *************************************/
  window::sliding_ar<uint32_t> ar(10s);
  std::vector<std::thread> consumers;

  for (size_t i = 0; i < 5; ++i) {
    consumers.emplace_back([&] {
      while (auto line = queue.pop()) {
        json j = json::parse(*line);

        // Apply detection rule for ultra-fertile processes
        if (j["type"] == "ProcessStarted") {
          ar.add(utils::unix2tp(j["ts"]), j["ppid"], j["pid"]);
        }
      }
    });
  }

  producer1.join();
  queue.close();

  for (auto& consumer : consumers) {
    consumer.join();
  }
}
