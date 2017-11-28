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
	
  char hostn[400]; // ��� ����� �������

  struct hostent *hostIP; // ������� �����
	
  // ������� �������� �� ����� ����� ��� ��������
  if((gethostname(hostn, sizeof(hostn))) == 0) {
    hostIP = gethostbyname(hostn);
  } else {
    printf("ERROR: - IP Address not found.");
  }

  if (argc != 2) {
    fprintf(stderr,"usage: ./client hostname port\n");
    exit(EXIT_FAILURE);
  }

  int port = argc > 2 ? atoi(argv[2]) : 7777; /* ����. � - ������.*/
	
  if ((he=gethostbyname(argv[1])) == NULL) { //����� ���� � �����
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

  if (connect(sockfd, (struct sockaddr *)&their_addr, sizeof(struct sockaddr)) == -1) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  if ((numbytes=recv(sockfd, buf, MAXDATASIZE-1, 0)) == -1) {
    perror("recv");
    exit(EXIT_FAILURE);
  }
  buf[numbytes] = 0;

#if 1
  printf("\n\nLocalhost: %s\n", inet_ntoa(*(struct in_addr *)hostIP->h_addr));
  printf("Local Port: %d\n", port);
  printf("Remote Host: %s\n", inet_ntoa(their_addr.sin_addr));
  printf("Received daytime data: %s\n",buf);
#endif

  close(sockfd);
  return 0;
}
