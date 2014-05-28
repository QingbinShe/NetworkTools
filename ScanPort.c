#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

#define MAXLINE 4098
#define SERV_PORT 9877
#define MAX_LINKE 1000

int main(int argc, char **argv)
{
  int sockfd, n;
  struct sockaddr_in servaddr;

  if (argc != 2) {
    printf("usage: <IPaddress>\n");
    return -1;
  }

  int i;
  for (i = 0; i < MAX_LINKE; i++) {
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
      printf("socket error\n");
      return -1;
    }

  //include in string.h
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(i);
    //inet_pton include in <arpa/inet.h>
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
      printf("inet_pton error\n");
      return -1;
    }
    
    if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
      close(sockfd);
      continue;
    }
    else {
      printf("useful port: %d\n", i);
    }
    close(sockfd);
  }

  exit(0);
}
