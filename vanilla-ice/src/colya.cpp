#include <print>
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

  // HTTP
  httplib::Server server;

  /**********************************************************
   *  HTTPS, TLS via OpenSSL be like:
   *    httplib::SSLServer server("./cert.pem", "./key.pem");
   *  HTTPS, mTLS via OpenSSL be like:
   *    httplib::SSLServer server("./cert.pem", "./key.pem",
   *                              "./client-ca-cert.pem");
   *********************************************************/

  std::print(stdout, "Collector `colya` is listening to...\n");

  // listen for events
  server.Post("/event", [](const httplib::Request& req, httplib::Response&) {
    std::print("{}\n", req.body);
  });

  server.listen(host, port);
  return 0;
}
