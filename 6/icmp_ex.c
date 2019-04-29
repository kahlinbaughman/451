#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <netinet/in.h>
#include <netinet/ip.h> //ip hdeader library (must come before ip_icmp.h)
#include <netinet/ip_icmp.h> //icmp header
#include <arpa/inet.h> //internet address library
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>

#include "checksum.h" //my checksum library

#define BUFSIZE 1500 //1500 MTU (so within one frame in layer 2)
#define PROTO_ICMP 1

int transmitted = 0;
int received = 0;
int maxttl = 20;
char ** arg;
int wait = 1;
unsigned long min = 0;
unsigned long max = 0;
unsigned long avg = 0;

void handler(int signum)
{ //signal handler
    printf("--- %s --- %d packets transmitted, %d received, %d%% packet loss, time %lums rtt min/avg/max/mdev = %lu/%lu/%lu/%d\n", arg[1], transmitted, received, (((received-transmitted)/transmitted)/100), avg, min, (avg/transmitted), max, 5);
    exit(1); //exit after printing
}

void busyWait(int signum)
{
    wait = 0;
}

int main(int argc, char * argv[]){
  char sendbuf[BUFSIZE], recvbuf[BUFSIZE], controlbuf[BUFSIZE];
  struct icmp * icmp;
  struct ip * ip;
  int sockfd, ttl;
  int packet_len, recv_len, ip_len, data_len;
  unsigned long finalTime;
  struct timeval secondsF, secondsL;
  struct addrinfo * ai;
  struct iovec iov;
  struct msghdr msg;
  char *dst, dontchange[100];


  arg = argv;
  ttl = 1;

  if(argc > 2){
      if(strcmp(argv[2], "-m") == 0){
          if(argc >= 4)
          {
            if(strcmp(argv[3], "-f") == 0){
                ttl = atoi(argv[4]);
            }else{
                maxttl = atoi(argv[3]);
            }
        }else
        {
            maxttl = 30;
        }
      }
      if(strcmp(argv[2], "-f") == 0){
          if (argc < 4) {
              printf("-f requires an operand i.e '-f 5'\n");
              return 0;
          }
          if(argc >= 5)
          {
            ttl = atoi(argv[3]);
            if(argc >= 6){
                maxttl = atoi(argv[5]);
            }
            else{
                maxttl = 30;
            }
        }else
        {
            ttl = atoi(argv[3]);
        }
      }
  }



  //process addr info
  getaddrinfo(argv[1], NULL, NULL, &ai);



  //process destination addre
  printf("Dest: %s\n", ai->ai_canonname ? ai->ai_canonname : argv[1]);

  //Initialize the socket
  if((sockfd = socket(AF_INET, SOCK_RAW, PROTO_ICMP)) < 0){
    perror("socket"); //check for errors
    exit(1);
  }

  setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
  struct sockaddr_in *addr;
  addr = (struct sockaddr_in *)ai->ai_addr;
  dst = (inet_ntoa((struct in_addr)addr->sin_addr));
  strcpy(dontchange, dst);

  struct timeval timeout;
  timeout.tv_sec = 1;
  timeout.tv_usec = 0;


  while(1)
  {
      setsockopt(sockfd, IPPROTO_IP, IP_TTL, &ttl, sizeof(int));
      setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof timeout);
      signal(SIGINT,handler);

      //initiate ICMP header
      icmp = (struct icmp *) sendbuf; //map to get proper layout
      icmp->icmp_type = ICMP_ECHO; //Do an echoreply
      icmp->icmp_code = 0;
      icmp->icmp_id = 42;
      icmp->icmp_seq= 0;
      icmp->icmp_cksum = 0;

      //compute checksum
      icmp->icmp_cksum = checksum((unsigned short *) icmp, sizeof(struct icmp));

      packet_len = sizeof(struct icmp);

      gettimeofday(&secondsF, NULL);
      //send the packet
      if( sendto(sockfd, sendbuf, packet_len, 0, ai->ai_addr, ai->ai_addrlen) < 0){
        perror("sendto");//error check
        exit(1);
      }
      transmitted++;
      //built msgheader structure for receiving reply
      iov.iov_base = recvbuf;
      iov.iov_len = BUFSIZE;

      msg.msg_name = NULL;
      msg.msg_namelen = 0;
      msg.msg_iov = &iov;
      msg.msg_iovlen = 1;
      msg.msg_control=controlbuf;
      msg.msg_controllen=BUFSIZE;


      //recv the reply
      recv_len = recvmsg(sockfd, &msg, 0); //could get interupted ??
      if (recv_len >= 0) {
          received++;
      }



      gettimeofday(&secondsL, NULL);

      finalTime = ((secondsL.tv_usec - secondsF.tv_usec) /1000);

      if(min == 0)
      {
         min = finalTime;
      }
      if (finalTime > max) {
          max = finalTime;
      }
      if (finalTime < min) {
          min = finalTime;
      }

      avg += finalTime;

      // printf("%d\n",recv_len);

      ip = (struct ip*) recvbuf;
      ip_len = ip->ip_hl << 2; //length of ip header

      icmp = (struct icmp *) (recvbuf + ip_len);
      data_len = (recv_len - ip_len);

      if(recv_len < 0){
        printf("%d.|--** %.2ld ms\n", ttl, finalTime);
      } else {
        printf("%d.|--%s %.2ld ms\n", ttl, inet_ntoa(ip->ip_src), finalTime);
      }

      ttl++;

      if (strcmp((inet_ntoa(ip->ip_src)), dontchange) == 0 || ttl > maxttl){
        handler(1);
      }

      signal(SIGALRM,busyWait);
      alarm(1);
      while(wait);
      wait = 1;
    }
  return 0;
}
