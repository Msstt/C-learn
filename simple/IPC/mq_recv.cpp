#include <cstring>
#include <iostream>
#include <sys/ipc.h>
#include <sys/msg.h>

struct message {
  long type;
  char data[100];
};

int main() {
  key_t key = ftok("my_mqueue", 0);
  int mqid = msgget(key, 0666 | IPC_CREAT);
  message msg;
  msgrcv(mqid, &msg, sizeof(msg.data), 1, 0);
  std::cout << msg.data << std::endl;
  msgctl(mqid, IPC_RMID, NULL);
  return 0;
}
