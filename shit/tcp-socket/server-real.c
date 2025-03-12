#include "/usr/include/arpa/inet.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int server_fd, client_d;
  struct sockaddr_in address;
  char buffer[BUFFER_SIZE] = {0};
  int addrlen = sizeof(address);

  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_family = AF_INET;
  address.sin_port = htons(PORT);
  address.sin_addr.s_addr = INADDR_ANY;

  bind(server_fd, (struct sockaddr *)&address, addrlen);

  listen(server_fd, 3);
  printf("server running in port 8080");
  client_d =
      accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);

  printf("Client Connected\n");
  while (1) {

    memset(buffer, 0, BUFFER_SIZE);
    if (recv(client_d, buffer, BUFFER_SIZE, 0) <= 0) {
      break;
    }
    printf("received %s", buffer);
    send(client_d, "message received \n", 17, 0);
    if (strncmp(buffer, "exit", 4) == 0) {
      break;
    }
  }
  close(client_d);
  close(server_fd);
  return 0;
}
