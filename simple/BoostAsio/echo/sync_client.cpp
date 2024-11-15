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
  ip::tcp::socket client(service);
  client.connect(
      ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8080));
  std::string send_data;
  std::cin >> send_data;
  send_data += "\n";
  client.write_some(buffer(send_data + "\n"));
  send_data.pop_back();
  char buf[BUFFER_SIZE];
  int bytes = read(client, buffer(buf, BUFFER_SIZE),
                   std::bind(ReadComplete, buf, _1, _2));
  std::string recv_data(buf, bytes - 1);
  std::cout << "recv: " << recv_data << " ,data is "
            << (send_data == recv_data ? "ok" : "err") << std::endl;
  return 0;
}