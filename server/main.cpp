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
#include <iostream>

std::vector<std::string> clientNames;
std::vector<int>         clientSockets;

unsigned int serversockfd;

bool serverStarted = false;

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

void* connectionListener(void *arg)
{
  char** arguments = (char**)arg;
  unsigned int newsockfd, portno, clilen, n;
  char buffer[256];
  struct sockaddr_in serv_addr, cli_addr;

  serversockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(serversockfd < 0)
    printf("Error Opening Socket\n");

  portno = std::stoi(arguments[1]);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(portno);
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  if(bind(serversockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {
    printf("Error Binding Socket\n");
    pthread_exit(NULL);
  }

  listen(serversockfd, 5);

  serverStarted = true;

  while(true)
  {
    clilen = sizeof(cli_addr);
    newsockfd = accept(serversockfd, (struct sockaddr *)&cli_addr, &clilen);
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

int main(int argc, char *argv[])
{
  if(argc < 2)
  {
    printf("Usage: <port>\n");
    return 1;
  }

  pthread_t listenerThread;
  pthread_create(&listenerThread, NULL, connectionListener, (void*)argv);

  sleep(1);
  if(!serverStarted)
  {
    std::cout << "Failed to Start Server\n";
    return 1;
  }
  std::cout << "Server Started\n";

  while(true)
  {
    char command[256];
    std::cin >> command;
    if(strcmp(command, "stop") == 0)
      break;
  }
  std::cout << "Stopping Server\n";

  close(serversockfd);
  for(int i = 0; i < clientSockets.size(); ++i)
    close(clientSockets.at(i));
  pthread_cancel(listenerThread);
}
