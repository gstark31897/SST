#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void* listener(void* arg)
{
  int sockfd = *((int*)arg);
  printf("SOCKFD:%i\n", &sockfd);
}

void* writer(void* arg)
{

}

int main(int argc, char *argv[])
{
  int sockfd, portno, n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  if(argc < 4)
  {
    printf("Usage: <hostname> <port> <username>\n");
    return 1;
  }

  portno = atoi(argv[2]);
  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
  {
    printf("Error Opening Socket\n");
    return 1;
  }

  server = gethostbyname(argv[1]);
  if(server == NULL)
  {
    printf("Error Connecting to Host\n");
    return 1;
  }

  bzero((char*)&serv_addr, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);
  serv_addr.sin_port = htons(portno);
  if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("Error Connection to Server\n");
    return 1;
  }

  char* buffer = argv[3];
  n = write(sockfd, buffer, strlen(buffer));
  if(n < 0)
  {
    printf("Error writing to socket\n");
    return 1;
  }

  n = read(sockfd, buffer, 256);
  if(n < 0)
  {
    printf("Error reading from socket\n");
    return 1;
  }
  printf("%s\n", buffer);

  pthread_t listenerThread;
  pthread_create(&listenerThread, NULL, listener, &sockfd);

  return 0;
}
