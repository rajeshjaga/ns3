#include "/usr/include/arpa/inet.h"
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int sock = 0;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE] = {0};

  sock = socket(AF_INET, SOCK_STREAM, 0);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

  while (1) {
    printf("Enter the message: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    send(sock, buffer, strlen(buffer), 0);
    memset(buffer, 0, BUFFER_SIZE);

    recv(sock, buffer, BUFFER_SIZE, 0);
    printf("Server Reply: %s", buffer);
  }

  close(sock);
  return 0;
}
