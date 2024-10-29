#include <arpa/inet.h>
#include <cstring>
#include <iostream>
#include <signal.h>
#include <sys/select.h>
#include <unistd.h>
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 30

int server_fd;

void handler(int num) {
  close(server_fd);
  exit(0);
}

int main() {
  signal(SIGINT, handler);
  server_fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in address;
  size_t address_len = sizeof(address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);
  bind(server_fd, reinterpret_cast<sockaddr *>(&address), address_len);
  listen(server_fd, 5);
  fd_set read_fds;
  int client_cnt = 0;
  int client_fd[MAX_CLIENTS];
  memset(client_fd, -1, sizeof(client_fd));
  char buffer[BUFFER_SIZE];
  while (true) {
    FD_ZERO(&read_fds);
    FD_SET(server_fd, &read_fds);
    int max_fd = server_fd;
    for (size_t i = 0; i < MAX_CLIENTS; i++) {
      if (client_fd[i] != -1) {
        FD_SET(client_fd[i], &read_fds);
        max_fd = std::max(max_fd, client_fd[i]);
      }
    }
    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
      std::cerr << "select error" << std::endl;
      return -1;
    }
    if (FD_ISSET(server_fd, &read_fds) && client_cnt < MAX_CLIENTS) {
      int new_client_fd =
          accept(server_fd, reinterpret_cast<sockaddr *>(&address),
                 reinterpret_cast<socklen_t *>(&address_len));
      std::cout << "Connect " << inet_ntoa(address.sin_addr) << " ..."
                << std::endl;
      write(new_client_fd, "Welcome ...", BUFFER_SIZE);
      for (size_t i = 0; i < MAX_CLIENTS; i++) {
        if (client_fd[i] == -1) {
          client_cnt++;
          client_fd[i] = new_client_fd;
	  break;
        }
      }
    }
    for (int i = 0; i < MAX_CLIENTS; i++) {
      if (FD_ISSET(client_fd[i], &read_fds)) {
        if (read(client_fd[i], buffer, BUFFER_SIZE) == 0) {
          close(client_fd[i]);
          client_cnt--;
          client_fd[i] = -1;
	  continue;
        }
        std::cout << buffer << std::endl;
        std::cin >> buffer;
        write(client_fd[i], buffer, BUFFER_SIZE);
      }
    }
  }
}
