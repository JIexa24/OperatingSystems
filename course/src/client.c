#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <assert.h>
#include <signal.h>
#include "../include/screen.h"
#define MAXDATASIZE 500 // Буфер приема
int sockfd;
static struct sigaction act;
static struct sigaction old;
static sigset_t set;
void ursignalHand(int sig)
{
  if (send(sockfd, "disconnect", 10, 0) == -1)
    perror("send");
  mt_clrscr();
  mt_gotoXY(1,1);
  printf("Disconnect\n");
  rk_mytermrestore();
  exit(0);
}

void setSignals()
{
  int i;
  memset(&(act), 0, sizeof(act));
  memset(&(old), 0, sizeof(old));
  sigemptyset(&set);
  sigaddset(&set, SIGUSR1);
  sigaddset(&set, SIGINT);
  act.sa_handler = ursignalHand;
  act.sa_mask = set;
  sigaction(SIGINT, &(act), &(old));   /*Ctrl-C*/
}

int main(int argc, char** argv)
{
  char field[10] = "         ";
  int numbytes;
  char buf[MAXDATASIZE];
  struct hostent *he;
  struct sockaddr_in their_addr; // Адрес сервака
  char hostn[400] = "127.0.0.1"; // Имя хоста текущее
  int port = 7777; /* Порт. Л - Логика.*/
  int opt, XY, k = 0;
  int cursorY = 0, cursorX = 0, position = 0;
  enum keys key;
  int exitw = 0;
  int refresh = 1;
  struct hostent *hostIP; // Айпишка хоста
  rk_mytermsave();
  setSignals();
  mt_clrscr();
  // Попытка получить по имени хоста его айпишник
  if((gethostname(hostn, sizeof(hostn))) == 0) {
    hostIP = gethostbyname(hostn);
  } else {
    printf("ERROR: - IP Address not found.");
  }

  opterr = 0;

  while ((opt = getopt(argc, argv, "p:h:")) != -1) {
    switch (opt) {
      case 'p':
        port = atoi(optarg);
      break;
      case 'h':
        strcpy(hostn, optarg);
      break;
    }
  }
  printf("port %d\n", port);
  printf("host %s\n", hostn);

  if ((he=gethostbyname(hostn)) == NULL) { //Ловим инфу о хосте
    perror("gethostbyname");
    exit(EXIT_FAILURE);
  }

  if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
    perror("socket");
    exit(1);
  }

  //Ставим инуфу о серваке
  their_addr.sin_family = AF_INET;
  their_addr.sin_port = htons(port);
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  memset(&(their_addr.sin_zero), 0, 8);
  char sendbuf[MAXDATASIZE];
  int i = 0,e,b;
  if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  for (i = 0; i < 2; i++) {
    numbytes = 0;
    if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
      perror("recv");
      exit(EXIT_FAILURE);
    }
    buf[numbytes] = 0;
    printf("From server: %s\n", buf);
    if (i == 1) {
      if (buf[numbytes - 1] == 'X'){
        XY = 1;
      } else XY = 0;
    }
  }
  printf("I have %s\n", XY == 1 ? "X" : "O");

  for (k = XY; k < 2; k ^= 1) {
    numbytes = 0;
    if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
      perror("recv");
      exit(EXIT_FAILURE);
    }
    buf[numbytes] = 0;
    //printf("\n\nLocalhost: %s\n", inet_ntoa(*(struct in_addr *)hostIP->h_addr));
    //printf("Local Port: %d\n", port);
    //printf("Remote Host: %s\n", inet_ntoa(their_addr.sin_addr));
    //printf("Received data: %s\n",buf);
    if (strcmp(buf,"Disconnected") == 0 || strcmp(buf,"Your Enemy Disconnected. Fail") == 0) {
      mt_clrscr();
      printf("Received data: %s\n",buf);
      mt_gotoXY(1,2);
      rk_mytermrestore();
      break;
    } else if (strcmp(buf,"Draw") == 0) {
    numbytes = 0;
    if ((numbytes=recv(sockfd, field, MAXDATASIZE-1, 0)) == -1) {
      perror("recv");
      exit(EXIT_FAILURE);
    }
    field[9] = 0;
    printField(field, XY, position);
    mt_gotoXY(1,30);
    printf("Received data: %s\n",buf);
    rk_mytermrestore();
    break;
    } else if (strcmp(buf,"You lose.") == 0) {
    numbytes = 0;
    if ((numbytes=recv(sockfd, field, MAXDATASIZE-1, 0)) == -1) {
      perror("recv");
      exit(EXIT_FAILURE);
    }
    field[9] = 0;
    printField(field, XY, position);
    mt_gotoXY(1,30);
    printf("Received data: %s\n",buf);
    rk_mytermrestore();
    break;
    } else if (strcmp(buf,"You win.") == 0) {
    numbytes = 0;
    if ((numbytes=recv(sockfd, field, MAXDATASIZE-1, 0)) == -1) {
      perror("recv");
      exit(EXIT_FAILURE);
    }
    field[9] = 0;
    printField(field, XY, position);
    mt_gotoXY(1,30);
    printf("Received data: %s\n",buf);
    rk_mytermrestore();
    break;
    } else {
      for (e = 8,b = numbytes - 1; e >= 0; e--, b--) {
        field[e] = buf[b];
      }
      /*printf("Field:\n");
      for (e = 0; e < 9; e++) {
        if (e % 3 == 0) {
          printf("\n");
        }
        printf("%c", field[e]);
      }
      printf("\n");*/

      refresh = 1;
      exitw = 0;
      printField(field, XY, position);
      if (k == 1) {
        while (exitw == 0) {
          position = cursorY * 3 + cursorX;
          if (refresh == 1) {
            printField(field, XY, position);
            refresh = 0;
          }

          rk_readkey(&key);
          switch (key) {
            case KEY_left:
              cursorX = ((cursorX - 1) + 3) % 3;
              key = KEY_other;
              refresh = 1;
            break;

            case KEY_right:
              cursorX = (cursorX + 1) % 3;
              key = KEY_other;
              refresh = 1;
            break;

            case KEY_down:
            cursorY = (cursorY + 1) % 3;
              key = KEY_other;
            refresh = 1;
            break;

            case KEY_up:
              cursorY = ((cursorY - 1) + 3) % 3;
              key = KEY_other;
              refresh = 1;
            break;

            case KEY_enter:
              key = KEY_other;
              if (field[position] == 'a') {
                exitw = 1;
              } else {
                continue;
                refresh = 0;
              }
            refresh = 0;
            break;
            default: refresh = 0; break;
          }
        }
        rk_mytermrestore();
        i = 2;
        //printf("Send msg: ");
        //do {
        //  scanf("%c", &sendbuf[i]);
        //} while (sendbuf[i++] != '\n');
        //sendbuf[i - 1] = '\0';
        sendbuf[0] = '0' + position;
        sendbuf[1] = '\0';
       if (send(sockfd, sendbuf, i, 0) == -1)
        perror("send");
        sendbuf[0] = '\0';
     }
   }
  } //for
  rk_mytermrestore();
  close(sockfd);
  return 0;
}
