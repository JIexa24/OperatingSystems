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
#include "../include/screen.h"

#define MAXDATASIZE 500 // Буфер приема

int main(int argc, char** argv)
{
  char field[10] = "         ";
  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct hostent *he;
  struct sockaddr_in their_addr; // Адрес сервака
  char hostn[400] = "127.0.0.1"; // Имя хоста текущее
  int port = 7777; /* Порт. Л - Логика.*/
  int opt, XY, k = 0;
  struct hostent *hostIP; // Айпишка хоста

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

  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
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
    printf("\n\nLocalhost: %s\n", inet_ntoa(*(struct in_addr *)hostIP->h_addr));
    printf("Local Port: %d\n", port);
    printf("Remote Host: %s\n", inet_ntoa(their_addr.sin_addr));
    printf("Received data: %s\n",buf);
    if (strcmp(buf,"Disconnected") == 0 || strcmp(buf,"Your Enemy Disconnected. Fail") == 0) {
      break;
    } else {
      for (e = 8,b = numbytes - 1; e >= 0; e--, b--) {
        field[e] = buf[b];
      }
      printField(field, XY);
      /*printf("Field:\n");
      for (e = 0; e < 9; e++) {
        if (e % 3 == 0) {
          printf("\n");
        }
        printf("%c", field[e]);
      }
      printf("\n");*/
    }

    if (k == 1) {
      i = 0;
      printf("Send msg: ");
      do {
        scanf("%c", &sendbuf[i]);
      } while (sendbuf[i++] != '\n');
      sendbuf[i - 1] = '\0';

      if (send(sockfd, sendbuf, i - 1, 0) == -1)
        perror("send");
    }
  }//for
  close(sockfd);
  return 0;
}
