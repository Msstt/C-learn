#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::system;

#define BUFFER_SIZE 1024

int main() {
  io_service service;
  ip::udp::socket socket(service, ip::udp::endpoint(ip::udp::v4(), 0));
  std::string send_data;
  std::cin >> send_data;
  ip::udp::endpoint endpoint(ip::address::from_string("127.0.0.1"), 8080);
  socket.send_to(buffer(send_data), endpoint);
  char buf[BUFFER_SIZE];
  ip::udp::endpoint sender_endpoint;
  int bytes = socket.receive_from(buffer(buf, BUFFER_SIZE), sender_endpoint);
  std::string recv_data(buf, bytes);
  std::cout << "recv: " << recv_data << " ,data is "
            << (send_data == recv_data ? "ok" : "err") << std::endl;
  return 0;
}