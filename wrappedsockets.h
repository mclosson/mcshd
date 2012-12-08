#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <string.h>
#include <syslog.h>

u_char no = 0;
u_int yes = 1;

void setTTL(int, int *);
void disableMulticastLoopback(int *);
void enableSocketReuse(int *);
void enableMulticastLoop(int *);
void bindSocket(int *, struct sockaddr_in);
void allocateSocket(int *);
void joinMulticastGroup(struct in_addr, int *);
void getSystemNodeName();
void sendToSocket(int *, char *, struct sockaddr_in);
void initializeIPAddress(struct sockaddr_in *, char *, unsigned int);

void setTTL(int ttl, int *sock)
{
  if (setsockopt(*sock, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl)) < 0) {
    perror("ttl setsockopt");
    exit(1);
  }
}

void disableMulticastLoopback(int *sock)
{
  if (setsockopt(*sock, IPPROTO_IP, IP_MULTICAST_LOOP, &no, sizeof(no)) < 0) {
    perror ("loop setsockopt");
    exit(1);
  }
} 

void enableSocketReuse(int *sock)
{
  if (setsockopt(*sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0) {
    perror("reuseaddr setsockopt");
    exit(1);
  }
}

void bindSocket(int *sock, struct sockaddr_in mcast_group)
{
  if (bind(*sock, (struct sockaddr*)&mcast_group, sizeof(mcast_group)) < 0) {
    perror ("bind");
    exit(1);
  }
}

void allocateSocket(int *sock)
{
  if ((*sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("send socket"); /* genericize this message */
    exit(1);
  }
}

void joinMulticastGroup(struct in_addr mcast_address, int *sock)
{
  struct ip_mreq mreq;

  mreq.imr_multiaddr = mcast_address;
  mreq.imr_interface.s_addr = htonl(INADDR_ANY);

  if (setsockopt(*sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0) {
    perror ("add_membership setsockopt");
    exit(1);
  }
}

void getSystemNodeName(struct utsname *name)
{
  if (uname(name) < 0) {
    perror ("uname");
    exit(1);
  }
}

void enableMulticastLoop(int *sock)
{
  u_char enable = 1;

  if (setsockopt(*sock, IPPROTO_IP, IP_MULTICAST_LOOP, &enable, sizeof(enable)) < 0) {
    perror("multicast_loop setsockopt");
    exit(1);
  }
}

void sendToSocket(int *sock, char *message, struct sockaddr_in addr)
{
  if(sendto(*sock, message, strlen(message)+1, 0, (const struct sockaddr *)&addr, sizeof(addr)) == -1) {
    perror("sendto");
    exit(1);
  }
}

void initializeIPAddress(struct sockaddr_in *address, char *ip, unsigned int port) {
  memset(address, 0, sizeof(*address));
  address->sin_family = AF_INET;
  address->sin_addr.s_addr = inet_addr(ip);
  address->sin_port = htons(port);
}
