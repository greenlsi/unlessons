#ifndef UDP_SERVER_H
#define UDP_SERVER_H

// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

struct udp_server_t {
  int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in addr;
  socklen_t len;
};
typedef struct udp_server_t udp_server_t;

udp_server_t* udp_server_new (int port);
int udp_server_recv (udp_server_t* this, udp_client_t* cli);

struct udp_client_t {
  int sockfd;
  struct sockaddr_in addr;
  socklen_t len;
};
typedef struct udp_client_t udp_client_t;

udp_client_t* udp_client_new (int port);
int udp_client_send (udp_client_t* this, const char* buf, int len);

#endif
