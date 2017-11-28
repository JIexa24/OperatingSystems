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
#define MAXDATASIZE 500 // Буфер приема
#define BACKLOG 10 //максимальная длина очереди
void sigchld_handler(int s)
{
  while(wait(NULL) > 0);
}
int main(int argc, char** argv)
{
  int port = argc > 1 ? atoi(argv[1]) : 7777; // прослушиваемый порт
  time_t timer;
  int sockfd, new_fd[2], numbytes[2]; /* sock_fd - то, что слушаем
                           new_fd - для новых включений */
  struct sockaddr_in my_addr; // адрес хоста (сервера)
  struct sockaddr_in their_addr[2]; // Адрес подключившегося
  socklen_t sin_size;
  struct sigaction sa;
  int yes=1;
  char buf[2][MAXDATASIZE];


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
  
  int count = 0;
  while(1) {
    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd[0] = accept(sockfd, (struct sockaddr *)&their_addr[0],&sin_size)) == -1) {
      //perror("accept");
      continue;
    }
    count++;
    printf("Received request from Client %d: %s:%d\n",
      count,inet_ntoa(their_addr[0].sin_addr),port);

    if ((new_fd[1] = accept(sockfd, (struct sockaddr *)&their_addr[1],&sin_size)) == -1) {
      //perror("accept");
      continue;
    }
    count++;
    printf("Received request from Client %d: %s:%d\n",
      count,inet_ntoa(their_addr[1].sin_addr),port);

    if (!fork()) { // child process
      close(sockfd); // child doesn.t need the listener
      timer = time(NULL);
      srand(timer);
      int id[2];
      id[0] = rand() % 2;
      if (id[0] == 0) id[1] = 1; // 0 - o; 1 - x
      else id[1] = 0;            // 0 - x; 1 - o
      int k = 0,j = 0;
      //do {

      for (k = 0,j = 0; j < 2; k^=1,j++) { 
      if ((numbytes[k]=recv(new_fd[k], &buf[k], MAXDATASIZE-1, 0)) == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
      }
      buf[k][numbytes[k]] = 0;
      printf("message %d: %s\n",k+1,buf[k]);
      } 
      
      //for (k = 0,j = 0; j < 2; k^=1,j++) { 
      //  if ((numbytes[k]=send(new_fd[k], "I'm alive! Connect!\n", 20, 0)) == -1) {
      //    perror("send");
      //  }
      //}
      k = id[0] == 1 ? id[0] : id[1]; 
      for (k; k < 2; k^=1) {
        if ((numbytes[k]=send(new_fd[k], "Go\n", 3, 0)) == -1) {
          perror("send");
        }
        if ((numbytes[k]=recv(new_fd[k], buf[k], MAXDATASIZE-1, 0)) == -1) {
          perror("recv");
          exit(EXIT_FAILURE);
        }
        buf[k][numbytes[k]] = 0;
        printf("Received msg from %d: %s\n",k + 1, buf[k]);
        if (!strcmp(buf[k], "disconnect")) {
          if (send(new_fd[k], "Disconnected", 12, 0) == -1)
            perror("send");
            break;
        }// else if (send(new_fd[k], "I'm alive! Connect!\n", 20, 0) == -1)
         // perror("send");
      } // for 
      //} while (1);

      close(new_fd[0]);
      close(new_fd[1]);
      exit(0);
    }
    close(new_fd[0]); // parent doesn.t need this
    close(new_fd[1]); // parent doesn.t need this
  }
  return 0;
}
