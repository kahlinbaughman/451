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
  int sockfd;
  int packet_len, recv_len, ip_len, data_len;
  unsigned long finalTime;
  struct timeval secondsF, secondsL;
  struct addrinfo * ai;
  struct iovec iov;
  struct msghdr msg;


  arg = argv;


  //process addr info
  getaddrinfo(argv[1], NULL, NULL, &ai);



  //process destination address
  printf("Dest: %s\n", ai->ai_canonname ? ai->ai_canonname : argv[1]);

  //Initialize the socket
  if((sockfd = socket(AF_INET, SOCK_RAW, PROTO_ICMP)) < 0){
    perror("socket"); //check for errors
    exit(1);
  }

  printf("%s\n", "Before while loop, seg fault?");

  while(1)
  {
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
      printf("%s\n", "After incrment, seg fault?");
      //built msgheader structure for receiving reply
      iov.iov_base = recvbuf;
      iov.iov_len = BUFSIZE;

      msg.msg_name = NULL;
      msg.msg_namelen = 0;
      msg.msg_iov = &iov;
      msg.msg_iovlen = 1;
      msg.msg_control=controlbuf;
      msg.msg_controllen=BUFSIZE;
      printf("%s\n", "Before recv");
      //recv the reply
      if((recv_len = recvmsg(sockfd, &msg, 0)) < 0){ //could get interupted ??
        perror("recvmsg");
        exit(1);
      }
      printf("%s\n", "After recv");
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

      printf("%d bytes from %s:icmp_req=%d ttl=%d time=%lu ms\n", data_len, inet_ntoa(ip->ip_src), transmitted, ip->ip_ttl, finalTime);
      signal(SIGALRM,busyWait);
      alarm(1);
      while(wait);
      wait = 1;
    }
  return 0;
}
