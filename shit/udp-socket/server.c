#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define MAXLINE 1024

int main() {
  int sockfd;
  char buffer[MAXLINE];
  struct sockaddr_in server_addr, client_addr;
  socklen_t len = sizeof(client_addr);
  FILE *fp;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket failed");
    exit(1);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_port = htons(PORT);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind failed");
    exit(1);
  }
  recvfrom(sockfd, buffer, MAXLINE, 0, (struct sockaddr *)&client_addr, &len);
  fp = fopen(buffer, "wb");
  if (!fp) {
    perror("file open failed");
    exit(1);
  }
  while (1) {
    int n = recvfrom(sockfd, buffer, MAXLINE, 0,
                     (struct sockaddr *)&client_addr, &len);
    if (n <= 0) {
      break;
    }
    fwrite(buffer, 1, n, fp);
  }
  printf("file recveied \n");
  close(fp);
  close(sockfd);
  return 0;
}
