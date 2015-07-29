#include <stdio.h>
#include <iostream>
#include <string>
#include <strings.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[])
{
  unsigned int sockfd, newsockfd, portno, clilen, n;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
    printf("Error Opening Socket\n");

  bzero((char*)&serv_addr, sizeof(serv_addr));
  portno = std::stoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    printf("Error Binding Socket\n");

  listen(sockfd, 5);

  clilen = sizeof(cli_addr);
  newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
  if(newsockfd < 0)
    printf("Error on Accept\n");

  bzero(buffer, 256);
  n = read(newsockfd, buffer, 255);
  if(n < 0)
    printf("Error Reading From Socket\n");
  printf("Message: %s\n", buffer);
}
