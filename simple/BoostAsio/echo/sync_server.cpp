#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::system;
using namespace std::placeholders;

#define BUFFER_SIZE 1024

size_t ReadComplete(char *buffer, const error_code &err, size_t bytes) {
  if (err) {
    return 0;
  }
  if (std::find(buffer, buffer + bytes, '\n') < buffer + bytes) {
    return 0;
  } else {
    return 1;
  }
}

int main() {
  io_service service;
  ip::tcp::acceptor server(service, ip::tcp::endpoint(ip::tcp::v4(), 8080));
  char buf[BUFFER_SIZE];
  while (true) {
    ip::tcp::socket client(service);
    server.accept(client);
    int bytes = read(client, buffer(buf, BUFFER_SIZE),
                     std::bind(ReadComplete, buf, _1, _2));
    std::string data(buf, bytes);
    client.write_some(buffer(data));
    client.close();
  }
  return 0;
}