#include <arpa/inet.h>
#include <iostream>
#include <liburing.h>
#include <signal.h>
#include <unistd.h>
#define BUFFER_SIZE 1024

enum class IO_TYPE { ACCEPTOR, CLIENT };

struct IOData {
  IO_TYPE type;
  int fd;
  char buffer[BUFFER_SIZE];
};

io_uring ring;
IOData server;
sockaddr_in address;
size_t address_len = sizeof(address);
bool is_running = true;

void submitAccept(int fd) {
  server.fd = fd;
  server.type = IO_TYPE::ACCEPTOR;
  io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  io_uring_prep_accept(sqe, fd, reinterpret_cast<sockaddr *>(&address),
                       reinterpret_cast<socklen_t *>(&address_len), 0);
  io_uring_sqe_set_data(sqe, &server);
  io_uring_submit(&ring);
}

void submitRead(int fd) {
  IOData *io_data = new IOData();
  io_data->fd = fd;
  io_data->type = IO_TYPE::CLIENT;
  io_uring_sqe *sqe = io_uring_get_sqe(&ring);
  io_uring_prep_read(sqe, fd, io_data->buffer, BUFFER_SIZE, 0);
  io_uring_sqe_set_data(sqe, io_data);
  io_uring_submit(&ring);
}

void handlerAccept(io_uring_cqe *cqe) {
  int new_client_fd = cqe->res;
  std::cout << "Connect " << inet_ntoa(address.sin_addr) << " ..." << std::endl;
  write(new_client_fd, "Welcome ...", BUFFER_SIZE);
  submitAccept(server.fd);
  submitRead(new_client_fd);
  io_uring_cqe_seen(&ring, cqe);
}

void handlerRead(io_uring_cqe *cqe) {
  IOData *io_data = reinterpret_cast<IOData *>(io_uring_cqe_get_data(cqe));
  int client_fd = io_data->fd;
  if (cqe->res == 0) {
    close(client_fd);
    std::cout << "Disconnect ..." << std::endl;
  } else {
    std::cout << io_data->buffer << std::endl;
    std::cin >> io_data->buffer;
    write(client_fd, io_data->buffer, BUFFER_SIZE);
    submitRead(client_fd);
  }
  io_uring_cqe_seen(&ring, cqe);
  delete io_data;
}

void handler(io_uring_cqe *cqe) {
  IOData *io_data = reinterpret_cast<IOData *>(io_uring_cqe_get_data(cqe));
  switch (io_data->type) {
  case IO_TYPE::ACCEPTOR:
    handlerAccept(cqe);
    break;
  case IO_TYPE::CLIENT:
    handlerRead(cqe);
    break;
  }
}

void stopProactor(int num) { is_running = false; }

void runProactor(int server_fd) {
  signal(SIGINT, stopProactor);
  io_uring_queue_init(256, &ring, 0);
  submitAccept(server_fd);
  while (is_running) {
    io_uring_submit_and_wait(&ring, 1);
    int head;
    io_uring_cqe *cqe;
    io_uring_for_each_cqe(&ring, head, cqe) { handler(cqe); }
  }
  io_uring_queue_exit(&ring);
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
  runProactor(server_fd);
  close(server_fd);
}