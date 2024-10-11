#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>

int main() {
  int client_fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in address;
  int address_len = sizeof(address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr("127.0.0.1");
  address.sin_port = htons(8080);
  if (connect(client_fd, reinterpret_cast<sockaddr *>(&address), address_len)) {
    std::cout << "Connect failed" << std::endl;
    return 0;
  }
  char data[1024];
  std::cin >> data;
  write(client_fd, data, 1024);
  close(client_fd);
  return 0;
}