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
#include <pthread.h>
#include <sys/wait.h>
#include <signal.h>
#include "../include/screen.h"
#define MYPORT 1025
#define MAXDATASIZE 500 // Буфер приема
#define BACKLOG 10 //максимальная длина очереди
#define GAMERS 2
#define WINCOORD 8
static char msghel[GAMERS][100] = {"You is a first gamer.\n","You is a second gamer.\n"};
static char win[3][100] = {"You win.","You lose.", "Draw"};
static char dis[100] = "Disconnected";
static char ydis[100] = "Your Enemy Disconnected. Fail";

static int wincoords[WINCOORD][3] = {{0,1,2},{3,4,5},{6,7,8},{0,3,6},{1,4,7},{2,5,8},{0,4,8},{2,4,6}};

static int flag = 1;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
void sigchld_handler(int s)
{
  pthread_mutex_lock(&mut);
  flag--;
  pthread_mutex_unlock(&mut);
  while(wait(NULL) > 0);
}

void checkFinal(char* field, int* winner)
{
  int i;
  int flagFieldIsFull = 1;
  *winner = -2;
  for (i = 0; i < WINCOORD; i++) {
    if (field[wincoords[i][0]] == field[wincoords[i][1]]){
      if (field[wincoords[i][1]] == field[wincoords[i][2]]){
        if (field[wincoords[i][0]] == 'X') *winner = 1;
        else if (field[wincoords[i][0]] == 'O') *winner = 0;
      }
    }
  }
  for (i = 0 ; i < 9; i++) {
    if (field[i] == 'a') {
      flagFieldIsFull = 0;
    }
  }
  if (flagFieldIsFull == 1 && *winner == -2) {
    *winner == -1; /*Draw*/
  }
}

int main(int argc, char** argv)
{
  int port = 7777; // прослушиваемый порт
  int games = 1;
  int winner = -2;
  int opt;
  time_t timer;
  int sockfd, new_fd[2], numbytes[2]; /* sock_fd - то, что слушаем
                           new_fd - для новых включений */
  struct sockaddr_in my_addr; // адрес хоста (сервера)
  struct sockaddr_in their_addr[2]; // Адрес подключившегося
  socklen_t sin_size[2];
  struct sigaction sa;
  int yes=1;
  char buf[2][MAXDATASIZE];
  int k = 0,j = 0;
  int idX,idO;

  opterr = 0;

  while ((opt = getopt(argc, argv, "p:g:")) != -1) {
    switch (opt) {
      case 'p':
        port = atoi(optarg);
      break;
      case 'g':
        games = atoi(optarg);
      break;
    }
  }
  printf("port %d\n", port);
  printf("games %d\n", games);
  flag = games;
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
  while(flag > 0) {
    for (k = 0,j = 0; j < GAMERS && games > 0; k^=1,j++) {
      sin_size[k] = sizeof(struct sockaddr_in);
      if ((new_fd[k] = accept(sockfd, (struct sockaddr *)&their_addr[k],&sin_size[k])) == -1) {
        //perror("accept");
        continue;
      }
      count++;
      printf("Received request from Client %d: %s:%d\n",
             count,inet_ntoa(their_addr[k].sin_addr),port);
      if ((send(new_fd[k], msghel[k], strlen(msghel[k]), 0)) == -1)
        perror("send");
    }
    if (games-- > 0)
    if (!fork()) { // child process
      usleep(1000000);
      close(sockfd); // child doesn.t need the listener
      char field[10] = "aaaaaaaaa";
      char tmp[100] = "You going ";
      timer = time(NULL);
      srand(timer);
      int id[2];
      id[0] = rand() % 2;
      if (id[0] == 0) id[1] = 1; // 0 - o; 1 - x
      else id[1] = 0;            // 0 - x; 1 - o
      k = 0,j = 0;
      //do {
      for (k = 0,j = 0; j < GAMERS; k^=1,j++) {
        strcat(msghel[k], "Your Enemy has IP: ");
        strcat(msghel[k], inet_ntoa(their_addr[k].sin_addr));
        strcat(msghel[k], "\nTo game: 1 sec.");
        strcat(msghel[k], "\nYour Symb: ");
        strcat(msghel[k], id[k] == 1 ? "X" : "O");
        if ((send(new_fd[k], msghel[k], strlen(msghel[k]), 0)) == -1)
          perror("send");
      }
      sleep(1);
      //for (k = 0,j = 0; j < 2; k^=1,j++) {
      //  if ((numbytes[k]=send(new_fd[k], "I'm alive! Connect!\n", 20, 0)) == -1) {
      //    perror("send");
      //  }
      //}
      int e = 0;
      strcat(tmp, field);
      k = id[0] == 1 ? 0 : 1;
      idX = k;
      idO = k^1;
      for (k;; k^=1) {
        for (e = 10; e < 19; e++){
          tmp[e] = field[e - 10];
        }
        if ((send(new_fd[k], tmp, 19, 0)) == -1)
          perror("send");
        if ((send(new_fd[k^1], field, 9, 0)) == -1)
          perror("send");

        if ((numbytes[k]=recv(new_fd[k], buf[k], MAXDATASIZE-1, 0)) == -1) {
          perror("recv");
          exit(EXIT_FAILURE);
        }
        buf[k][numbytes[k]] = 0;
        printf("Received msg from %d: %s\n",k + 1, buf[k]);
        if (strcmp(buf[k], "disconnect") == 0) {
          if (send(new_fd[k], dis, strlen(dis), 0) == -1)
            perror("send");
          if (send(new_fd[k^1], ydis, strlen(ydis), 0) == -1)
            perror("send");
            break;
        } else if (buf[k][numbytes[k] - 2] - '0' == 0 && field[0] == 'a') {
          field[0] = id[k] == 1 ? 'X' : 'O';
        } else if (buf[k][numbytes[k] - 2] - '0' == 1 && field[1] == 'a') {
          field[1] = id[k] == 1 ? 'X' : 'O';
        } else if (buf[k][numbytes[k] - 2] - '0' == 2 && field[2] == 'a') {
          field[2] = id[k] == 1 ? 'X' : 'O';
        } else if (buf[k][numbytes[k] - 2] - '0' == 3 && field[3] == 'a') {
          field[3] = id[k] == 1 ? 'X' : 'O';
        } else if (buf[k][numbytes[k] - 2] - '0' == 4 && field[4] == 'a') {
          field[4] = id[k] == 1 ? 'X' : 'O';
        } else if (buf[k][numbytes[k] - 2] - '0' == 5 && field[5] == 'a') {
          field[5] = id[k] == 1 ? 'X' : 'O';
        } else if (buf[k][numbytes[k] - 2] - '0' == 6 && field[6] == 'a') {
          field[6] = id[k] == 1 ? 'X' : 'O';
        } else if (buf[k][numbytes[k] - 2] - '0' == 7 && field[7] == 'a') {
          field[7] = id[k] == 1 ? 'X' : 'O';
        } else if (buf[k][numbytes[k] - 2] - '0' == 8 && field[8] == 'a') {
          field[8] = id[k] == 1 ? 'X' : 'O';
        }
        checkFinal(field, &winner);
        if (winner == 0) {
          if (send(new_fd[idO], win[0], strlen(win[0]), 0) == -1)
            perror("send");
          if (send(new_fd[idX], win[1], strlen(win[1]), 0) == -1)
            perror("send");
          break;
        } else if (winner == 1) {
          if (send(new_fd[idX], win[0], strlen(win[0]), 0) == -1)
            perror("send");
          if (send(new_fd[idO], win[1], strlen(win[1]), 0) == -1)
            perror("send");
          break;
        } else if (winner == -1) {
          if (send(new_fd[idO], win[2], strlen(win[2]), 0) == -1)
            perror("send");
          if (send(new_fd[idX], win[2], strlen(win[2]), 0) == -1)
            perror("send");
          break;
        }
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
