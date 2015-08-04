#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <string>

std::vector<std::string> clientNames;
std::vector<int>         clientSockets;

int findClient(std::string name)
{
  for(int i = 0; i < clientNames.size(); ++i)
  {
    if(!clientNames[i].compare(name))
      return clientSockets[i];
  }
  return 0;
}

void* clientListener(void *arg)
{
  char name[256];
  strcpy(name, clientNames.back().c_str());
  int sockfd = clientSockets.back();
  printf("User %s connected with fd of %i\n", name, sockfd);
  while(true)
  {
    char buffer[256];
    int n = read(sockfd, buffer, 256);
    if(n <= 0)
    {
      printf("User %s disconnected\n", name);
      break;
    }

    if(n < 3)
      continue;

    std::string temp(buffer);
    int split = temp.find(':');
    std::string receiver = temp.substr(0, split);
    std::string content  = temp.substr(split+1, temp.length()+1);
    std::string message(name);
    message += ":" + content;

    int receiverSocket = findClient(receiver);
    if(receiverSocket > 0)
    {
      n = send(receiverSocket, message.c_str(), 256, NULL);
    }
    else
    {
      n = send(sockfd, "User not found\0", 256, NULL);
    }
  }
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

    bzero(&buffer, 256);
    n = read(newsockfd, buffer, 256);
    if(n < 0)
      printf("Error Reading From Socket\n");

    std::string client(buffer);
    clientNames.push_back(client);
    clientSockets.push_back(newsockfd);

    pthread_t thread;
    pthread_create(&thread, NULL, clientListener, (void*)NULL);
  }
}
