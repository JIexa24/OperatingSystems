#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>

#include <sys/wait.h>
#include <signal.h>

#define MYPORT 1025 
#define BACKLOG 10 //максимальная длина очереди
void sigchld_handler(int s)
{
  while(wait(NULL) > 0);
}
int main(int argc, char** argv)
{
  int port = argc > 1 ? atoi(argv[1]) : 7777; // прослушиваемый порт
  time_t timer;
  int sockfd, new_fd; /* sock_fd - то, что слушаем
                           new_fd - для новых включений */
  struct sockaddr_in my_addr; // адрес хоста (сервера)
  struct sockaddr_in their_addr; // Адрес подключившегося
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
    
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {  
   perror("socket");
   exit(1);
  }

  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
    perror("setsockopt");
    exit(1);
  }  
	
  my_addr.sin_family = AF_INET;
  my_addr.sin_port = htons(port);
  my_addr.sin_addr.s_addr = INADDR_ANY;
  memset(&(my_addr.sin_zero), 0, 8);

  if (bind(sockfd, (struct sockaddr *)&my_addr, sizeof(struct sockaddr)) == -1) {
    perror("bind");
    exit(1);
  }

  if (listen(sockfd, BACKLOG) == -1) {
    perror("listen");
    exit(1);
  }
  sa.sa_handler = sigchld_handler; // reap all dead processes
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = SA_RESTART;
  if (sigaction(SIGCHLD, &sa, NULL) == -1) {
    perror("sigaction");
    exit(1);
  }

  while(1) {
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(sockfd, (struct sockaddr *)&their_addr,&sin_size)) == -1) {
      //perror("accept");
      continue;
    }
    printf("Received request from Client: %s:%d\n",
      inet_ntoa(their_addr.sin_addr),port);
    if (!fork()) { // child process
      close(sockfd); // child doesn.t need the listener
      timer = time(NULL);
      if (send(new_fd, ctime(&timer), 30, 0) == -1)
        perror("send");
      close(new_fd);
      exit(0);
    }
    close(new_fd); // parent doesn.t need this
  }
  return 0;
}
