#include <pthread.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

#include "../gui/gui.h"

pthread_t listenerthread;
pthread_t writerthread;

int sockfd;

void* listener(void* arg)
{
  int sockfd = *((int*)arg);
  while(true)
  {
    char message[256];
    int n = read(sockfd, message, 256);
    if(n <= 0)
    {
      printf("Server disconnected\n");
      break;
    }
    if(n < 1)
      continue;
    UpdateBuffer(message);
  }
  close(sockfd);
}

void* writer(void* arg)
{
  char *message = (char*)arg;
  write(sockfd, message, strlen(message));
}

void SendMessage(char *receiver, char *message)
{
  char      *fullmessage;
  pthread_t writerThread;

  fullmessage = (char*)malloc(sizeof(char) * (strlen(receiver) + strlen(message) + 2));
  strcpy(fullmessage, receiver);
  strcat(fullmessage, ":");
  strcat(fullmessage, message);

  pthread_create(&writerThread, NULL, writer, (void*)message);
}

int StartBackend(char *hostname, int portno, char *username)
{
  int n;
  struct sockaddr_in serv_addr;
  struct hostent *server;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
  {
    printf("Error Opening Socket\n");
    return 1;
  }

  server = gethostbyname(hostname);
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

  n = write(sockfd, username, strlen(username));
  if(n < 0)
  {
    printf("Error writing to socket\n");
    return 1;
  }

  pthread_create(&listenerthread, NULL, listener, (void*)&sockfd);

  return 0;
}

int StopBackend()
{
  pthread_cancel(listenerthread);
  close(sockfd);
  return 0;
}
