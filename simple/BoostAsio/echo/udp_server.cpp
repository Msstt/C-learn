#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::system;

#define BUFFER_SIZE 1024

int main() {
  io_service service;
  ip::udp::socket socket(service, ip::udp::endpoint(ip::udp::v4(), 8080));
  while (true) {
    char buf[BUFFER_SIZE];
    ip::udp::endpoint sender_endpoint;
    int bytes = socket.receive_from(buffer(buf, BUFFER_SIZE), sender_endpoint);
    std::cout << "recv from" << sender_endpoint.address().to_string() << ":"
              << sender_endpoint.port() << "..." << std::endl;
    std::string data(buf, bytes);
    socket.send_to(buffer(data), sender_endpoint);
  }
  return 0;
}