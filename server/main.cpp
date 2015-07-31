#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
std::map<char*, int> clients;

void* clientListener(void *arg)
{
  char* user = (char*)arg;
  int sockfd = clients[user];
  printf("User %s connected with fd of %i\n", user, clients[user]);
  while(true)
  {
    char message[256];
    int n = read(sockfd, message, 255);
    if(n <= 0)
    {
      printf("User %s disconnected\n", user);
      break;
    }

    if(n < 3)
      continue;

    printf("User %s send message %s\n", user, message);

    std::string temp(message);
    int split = temp.find(':');
    const char* receiver = temp.substr(0, split).c_str();
    printf(receiver);
    const char* send  = temp.substr(split+1, temp.length()).c_str();
    printf(send);
  }
  clients.erase(user);
  close(sockfd);
  pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("Usage: <port>\n");
    return 1;
  }

  unsigned int sockfd, newsockfd, portno, clilen, n;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0)
    printf("Error Opening Socket\n");

  portno = std::stoi(argv[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    printf("Error Binding Socket\n");

  listen(sockfd, 5);

  while(true)
  {
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if(newsockfd < 0)
      printf("Error on Accept\n");

    printf("Accepted Client");

    n = read(newsockfd, buffer, 255);
    if(n < 0)
      printf("Error Reading From Socket\n");
    printf("Message: %s\n", buffer);
    clients[buffer] = newsockfd;

    pthread_t thread;
    pthread_create(&thread, NULL, clientListener, (void*)buffer);
  }
}
