#include <format>
#include <iostream>
#include <sstream>

#define CPPHTTPLIB_OPENSSL_SUPPORT
#include "yhirose/httplib.hpp"

int main(int argc, char* argv[]) {
  // default arg values
  std::string host{"127.0.0.1"};
  int port{8080};

  // presented arg values
  if (argc >= 3) {
    host = argv[1];

    std::stringstream ss;
    ss << argv[2];
    ss >> port;
  }

  // address in "127.0.0.1:8080" format
  std::string address{std::format("{}:{}", host, port)};

  constexpr int timeout = 5;      // seconds
  constexpr int retry_count = 5;  // how many times

  /********************************************************
   * [As quick prototype]
   *  HTTPLib is not a production solution:
   *  1. Blocking I/O.
   *  2. Not designed for high concurrency.
   *  3. Not ideal for high-load servers.
   *  Just for demo purposes.
   *
   *  In a real-world we'd use, e.g.
   *  a) Boost.Beast + Boost.Asio;
   *  b) libcurl;
   *  Note: it'll be huge.
   *******************************************************/

  httplib::Client client(address);
  client.set_connection_timeout(timeout);
  client.set_read_timeout(timeout);

  // Headers are the same for all requests
  httplib::Headers headers = {
      {"Content-Type", "application/json"},
      {"Authorization", "Bearer TOKEN"}};  // it's JWT more often

  // Body will be JSON-line: R"({"name":"wazzup","level":"severe"})";

  // One-thread sender is enough to satisfy task
  std::string json_line;
  while (std::cin and std::getline(std::cin, json_line)) {
    if (json_line.length() == 0) {
      continue;
    }

    std::cout << "Received: " << json_line << std::endl;

    // Send with retry
    bool is_sent = false;

    for (int attempt = 0; attempt < retry_count; ++attempt) {
      auto res = client.Post("/event", headers, json_line, "application/json");

      if (res) {
        if (res->status == 200) {  // HTTP OK
          is_sent = true;
          std::cout << "Sent: " << json_line << '\n';
          break;
        } else {  // HTTP not OK
          std::cout << "Error: " << httplib::to_string(res.error()) << " for "
                    << json_line << '\n';
        }
      } else {
        // handle no response, connections are managed inside httplib
        std::cout << "no response from server: " << address << '\n';
      }

      // artificial sleep for 5 sec
      sleep(timeout);
    }

    if (not is_sent) {
      // There are different approaches what to do in this case,
      // as for demo, we do nothing but print to `stdout`
      std::cout << "Not sent: " << json_line << '\n';
    }
  }

  return 0;
}
