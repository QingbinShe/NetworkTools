#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<netdb.h>

#define MAXLINE 4098

int main(int argc, char **argv)
{
  int sockfd, n;
  struct sockaddr_in servaddr;

  //use for getaddrinfo
  struct addrinfo hints, *res, *ressave;

  //input the IP address or DNS and port(from argv[2] to argv[3])
  if (argc != 4) {
    printf("usage: fulfill the cmd\n");
    return -1;
  }

  int i;
  //atoi():char* to int
  for (i = atoi(argv[2]); i < atoi(argv[3]); i++) {
    //inet_pton include in <arpa/inet.h>
    if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
      //when input the DNS, use getaddrinfo
      bzero(&hints, sizeof(struct addrinfo));
      hints.ai_flags = AI_CANONNAME;
      hints.ai_family = AF_INET;
      hints.ai_socktype = SOCK_STREAM;
      char num[32];
      //change int(i) to char(num)
      sprintf(num, "%d", i);
      //printf("num is %s\n", num);
      if(getaddrinfo(argv[1], num, &hints, &res) != 0) {
	printf("getaddrinfo error\n");
        return -1;
      }
      ressave = res;
      for (; res != NULL; res = res -> ai_next) {
        if ((sockfd = socket(res -> ai_family, res -> ai_socktype, res -> ai_protocol)) < 0) {
	  continue;
	}
	if (connect(sockfd, res -> ai_addr, res -> ai_addrlen) == 0){
          printf("useful port: %d\n", i);
	  close(sockfd);
	  continue;
	}
	else {
	  close(sockfd);
	}
      }
      res = ressave;
      /*
      if (res == NULL) {
        printf("no address to connect\n");
	return -1;
      }
      freeaddrinfo(ressave);
      exit(0);
      */
    }
    else {
      //include in string.h
      bzero(&servaddr, sizeof(servaddr));
      servaddr.sin_family = AF_INET;
      servaddr.sin_port = htons(i);
      if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
          printf("socket error\n");
          return -1;
      }
   
      if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        //printf("unuseful port: %d\n", i);
        close(sockfd);
        continue;
      }
      else {
        printf("useful port: %d\n", i);
        close(sockfd);
        continue;
      }
    }
  }
  exit(0);
}
