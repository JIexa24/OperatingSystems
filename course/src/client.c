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

#define MAXDATASIZE 500 // ����� ������

int main(int argc, char** argv)
{
  int sockfd, numbytes;
  char buf[MAXDATASIZE];
  struct hostent *he;
  struct sockaddr_in their_addr; // ����� �������	
  char hostn[400] = "127.0.0.1"; // ��� ����� �������
  int port = 7777; /* ����. � - ������.*/
  int opt;
  struct hostent *hostIP; // ������� �����

  // ������� �������� �� ����� ����� ��� ��������
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

  if ((he=gethostbyname(hostn)) == NULL) { //����� ���� � �����
    perror("gethostbyname");
    exit(EXIT_FAILURE);
  }
	
  if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(1);
  }
  
  //������ ����� � �������
  their_addr.sin_family = AF_INET;
  their_addr.sin_port = htons(port);
  their_addr.sin_addr = *((struct in_addr *)he->h_addr);
  memset(&(their_addr.sin_zero), 0, 8);
  char sendbuf[MAXDATASIZE];
  int i = 0;
  if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  do {
  i = 0;
  printf("Send msg: ");
    do {
      scanf("%c", &sendbuf[i]);
    } while (sendbuf[i++] != '\n');
  sendbuf[i - 1] = '\0';

  if (send(sockfd, sendbuf, i - 1, 0) == -1)
    perror("send");

  if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    perror("recv");
    exit(EXIT_FAILURE);
  }
  buf[numbytes] = 0;

  printf("\n\nLocalhost: %s\n", inet_ntoa(*(struct in_addr *)hostIP->h_addr));
  printf("Local Port: %d\n", port);
  printf("Remote Host: %s\n", inet_ntoa(their_addr.sin_addr));
  printf("Received data: %s\n",buf);

  } while (strcmp(buf,"Disconnected") != 0 && strcmp(buf,"Your Enemy Disconnected. Fail") != 0);
  close(sockfd);
  return 0;
}
