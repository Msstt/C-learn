#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::system;
using namespace std::placeholders;

#define BUFFER_SIZE 1024
io_service service;

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

class Client {
public:
  Client(std::string user_name) : user_name_(user_name), socket_(service) {}

  void Connect(ip::tcp::endpoint endpoint) {
    this->socket_.connect(endpoint);
    this->Write("login " + user_name_ + "\n");
    this->ReadAnswer();
  }

  void Run() {
    std::string op;
    while (true) {
      std::cin >> op;
      this->WriteRequest();
      this->ReadAnswer();
    }
  }

private:
  void Write(std::string msg) { this->socket_.write_some(buffer(msg)); }

  void WriteRequest() { this->Write("ping\n"); }

  void ReadAnswer() {
    ready_bytes = read(this->socket_, buffer(this->buffer_, BUFFER_SIZE),
                       std::bind(ReadComplete, this->buffer_, _1, _2));
    this->ProcessMsg();
  }

  void ProcessMsg() {
    std::string msg(this->buffer_, this->ready_bytes);
    if (msg.find("login") == 0) {
      this->OnLogin();
    }
    if (msg.find("ping ") == 0) {
      this->OnPing(msg);
    }
    if (msg.find("clients ") == 0) {
      this->OnClients(msg);
    }
  }

  void OnLogin() { std::cout << "Login success!" << std::endl; }

  void OnPing(const std::string &msg) {
    if (msg == "ping ok\n") {
      std::cout << "Ping ok!" << std::endl;
    } else if (msg == "ping client_list_changed\n") {
      this->AskClient();
    } else {
      std::cerr << "Ping error!" << std::endl;
    }
  }

  void OnClients(const std::string &msg) {
    std::cout << "New client list: " << msg.substr(8) << std::endl;
  }

  void AskClient() {
    this->Write("ask_client\n");
    ReadAnswer();
  }

  std::string user_name_;
  ip::tcp::socket socket_;
  char buffer_[BUFFER_SIZE];
  int ready_bytes;
};

int main() {
  std::string user_name;
  std::cout << "Login name:";
  std::cin >> user_name;
  Client client(user_name);
  client.Connect(
      ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), 8080));
  client.Run();
  return 0;
}