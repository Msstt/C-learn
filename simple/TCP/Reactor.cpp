#include <arpa/inet.h>
#include <iostream>
#include <memory>
#include <signal.h>
#include <unistd.h>
#define BUFFER_SIZE 1024
#define MAX_CLIENTS 30

class Handler {
public:
  virtual void handler() = 0; // Event Handler (事件处理器)

  int fd_{-1}; // Handles (句柄)
};

int client_cnt = 0;
char buffer[BUFFER_SIZE];
fd_set read_fds;
std::unique_ptr<Handler> handlers[MAX_CLIENTS + 1];
bool is_running = true;

// Concrete Event Handlers (具体事件处理器)

class AccetorHandler : public Handler {
public:
  void handler() override {
    sockaddr_in address;
    size_t address_len = sizeof(address);
    int new_client_fd =
        accept(this->fd_, reinterpret_cast<sockaddr *>(&address),
               reinterpret_cast<socklen_t *>(&address_len));
    std::cout << "Connect " << inet_ntoa(address.sin_addr) << " ..."
              << std::endl;
    write(new_client_fd, "Welcome ...", BUFFER_SIZE);
    for (size_t i = 1; i <= MAX_CLIENTS; i++) {
      if (handlers[i]->fd_ == -1) {
        client_cnt++;
        handlers[i]->fd_ = new_client_fd;
        break;
      }
    }
  }
};

class ClientHandler : public Handler {
public:
  void handler() override {
    if (read(this->fd_, buffer, BUFFER_SIZE) == 0) {
      close(this->fd_);
      for (int i = 1; i <= MAX_CLIENTS; i++) {
        if (handlers[i]->fd_ == this->fd_) {
          client_cnt--;
          handlers[i]->fd_ = -1;
          break;
        }
      }
      std::cout << "Disconnect ..." << std::endl;
      return;
    }
    std::cout << buffer << std::endl;
    std::cin >> buffer;
    write(this->fd_, buffer, BUFFER_SIZE);
  }
};

void eventDemultiplexer() { // Synchronous Event Demultiplexer (事件多路分发器)
  FD_ZERO(&read_fds);
  int max_fd = 0;
  for (size_t i = 0; i <= MAX_CLIENTS; i++) {
    if (handlers[i]->fd_ != -1) {
      FD_SET(handlers[i]->fd_, &read_fds);
      max_fd = std::max(max_fd, handlers[i]->fd_);
    }
  }
  if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) == -1) {
    std::cerr << "select error" << std::endl;
    exit(0);
  }
  for (int i = 0; i <= MAX_CLIENTS; i++) {
    if (FD_ISSET(handlers[i]->fd_, &read_fds)) {
      handlers[i]->handler();
    }
  }
}

void stopReactor(int num) { is_running = false; }

void runReactor(int server_fd) { // Initiation Dispatcher (初始化分发器)
  signal(SIGINT, stopReactor);
  handlers[0] = std::make_unique<AccetorHandler>();
  handlers[0]->fd_ = server_fd;
  for (int i = 1; i <= MAX_CLIENTS; i++) {
    handlers[i] = std::make_unique<ClientHandler>();
    handlers[i]->fd_ = -1;
  }
  while (is_running) {
    eventDemultiplexer();
  }
}

int initServer() {
  int server_fd = socket(AF_INET, SOCK_STREAM, 0);
  sockaddr_in address;
  size_t address_len = sizeof(address);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons(8080);
  bind(server_fd, reinterpret_cast<sockaddr *>(&address), address_len);
  listen(server_fd, 5);
  return server_fd;
}

int main() {
  int server_fd = initServer();
  runReactor(server_fd);
  close(server_fd);
}