#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::system;
using namespace std::placeholders;

#define BUFFER_SIZE 1024
#define TIMEOUT_S 5

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

class Client;
std::mutex clients_mutex;
std::vector<std::shared_ptr<Client>> clients;

class Client {
public:
  Client() : socket_(service) {}
  ~Client() { this->Stop(); }

  ip::tcp::socket &Socket() { return this->socket_; }

  std::string UserName() { return this->user_name_; }

  void SetClientsChanged() { this->clients_changed_ = true; }

  void AnswerToClient() {
    try {
      this->ReadRequest();
      this->ProcessRequest();
    } catch (system_error &) {
      this->Stop();
    }
  }

  bool TimeOut() {
    auto now_time = std::chrono::system_clock::now();
    return this->is_closed_ ||
           now_time - this->last_ping_time_ > std::chrono::seconds(TIMEOUT_S);
  }

  void Stop() {
    if (this->is_closed_) {
      return;
    }
    std::cout << this->user_name_ << " logout..." << std::endl;
    this->is_closed_ = true;
    error_code err;
    this->socket_.close(err);
  }

private:
  void Write(std::string msg) { this->socket_.write_some(buffer(msg)); }

  void ReadRequest() {
    if (this->socket_.available()) {
      this->ready_bytes += this->socket_.read_some(buffer(
          this->buffer_ + this->ready_bytes, BUFFER_SIZE - this->ready_bytes));
    }
  }

  void ProcessRequest() {
    if (std::find(this->buffer_, this->buffer_ + this->ready_bytes, '\n') >=
        this->buffer_ + this->ready_bytes) {
      return;
    }
    size_t pos =
        std::find(this->buffer_, this->buffer_ + this->ready_bytes, '\n') -
        this->buffer_;
    std::string msg(this->buffer_, pos);
    std::copy(this->buffer_ + pos + 1, this->buffer_ + ready_bytes,
              this->buffer_);
    this->ready_bytes -= pos + 1;
    if (msg.find("login ") == 0) {
      this->OnLogin(msg);
    } else if (msg.find("ping") == 0) {
      this->OnPing();
    } else if (msg.find("ask_client") == 0) {
      this->OnClients();
    } else {
      std::cerr << "invalid msg: " << msg << std::endl;
    }
    this->last_ping_time_ = std::chrono::system_clock::now();
  }

  void OnLogin(const std::string &msg) {
    this->user_name_ = msg.substr(6);
    std::cout << this->user_name_ << " login..." << std::endl;
    this->Write("login\n");
  }

  void OnPing() {
    if (clients_changed_) {
      this->Write("ping client_list_changed\n");
    } else {
      this->Write("ping ok\n");
    }
  }

  void OnClients() {
    this->clients_changed_ = false;
    std::string msg = "clients ";
    for (const auto &client : clients) {
      msg += client->UserName() + " ";
    }
    msg += "\n";
    this->Write(msg);
  }

  std::string user_name_;
  ip::tcp::socket socket_;
  char buffer_[BUFFER_SIZE];
  int ready_bytes;
  bool clients_changed_{false};
  std::chrono::system_clock::time_point last_ping_time_;
  bool is_closed_{false};
};

std::atomic<bool> running{true};

void StopServer(const error_code &err, int signum) {
  std::cout << "Server close..." << std::endl;
  running = false;
}

void HandleAccept() {
  ip::tcp::acceptor server(service, ip::tcp::endpoint(ip::tcp::v4(), 8080));
  server.non_blocking(true);
  while (running) {
    std::shared_ptr<Client> client = std::make_shared<Client>();
    error_code err;
    do {
      server.accept(client->Socket(), err);
    } while (running && err);
    if (!err) {
      std::unique_lock<std::mutex> lock(clients_mutex);
      clients.push_back(client);
      for (const auto &client : clients) {
        client->SetClientsChanged();
      }
    }
  }
  server.close();
}

void HandleClients() {
  while (running) {
    std::unique_lock<std::mutex> lock(clients_mutex);
    auto client_cnt = clients.size();
    for (const auto &client : clients) {
      client->AnswerToClient();
    }
    clients.erase(std::remove_if(clients.begin(), clients.end(),
                                 std::bind(&Client::TimeOut, _1)),
                  clients.end());
    if (clients.size() != client_cnt) {
      for (const auto &client : clients) {
        client->SetClientsChanged();
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1));
  }
}

int main() {
  signal_set signal(service, SIGINT);
  signal.async_wait(StopServer);
  std::vector<std::thread> threads;
  threads.emplace_back(HandleAccept);
  threads.emplace_back(HandleClients);
  threads.emplace_back([]() { service.run(); });
  for (auto &thread : threads) {
    thread.join();
  }
  return 0;
}