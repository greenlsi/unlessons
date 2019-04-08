// Server side implementation of UDP client-server model
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT	 8080
#define MAXLINE 1024

struct udp_server_t {
  int sockfd;
	char buffer[MAXLINE];
	struct sockaddr_in addr;
  socklen_t len;
};
typedef struct udp_server_t udp_server_t;

struct udp_client_t {
  int sockfd;
  struct sockaddr_in addr;
  socklen_t len;
};
typedef struct udp_client_t udp_client_t;

udp_server_t*
udp_server_new (int port)
{
  udp_server_t* this = (udp_server_t*) malloc (sizeof (udp_server_t));
	if ( (this->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&this->addr, 0, sizeof(this->addr));
	this->addr.sin_family = AF_INET; // IPv4
	this->addr.sin_addr.s_addr = INADDR_ANY;
	this->addr.sin_port = htons(port);
	if ( bind(this->sockfd, (const struct sockaddr *)&this->addr,
            sizeof(this->addr)) < 0 ) {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  return this;
}

int
udp_server_recv (udp_server_t* this, udp_client_t* cli)
{
	int n = recvfrom (this->sockfd, this->buffer, MAXLINE,
                    MSG_WAITALL,
                    (struct sockaddr *) &cli->addr, &cli->len);
	this->buffer[n] = '\0';
	return n;
}

udp_client_t*
udp_client_new (const char* server, int port)
{
  udp_client_t* this = (udp_client_t*) malloc (sizeof (udp_client_t));
	if ( (this->sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
		perror("socket creation failed");
		exit(EXIT_FAILURE);
	}
	memset(&this->addr, 0, sizeof(this->addr));
	this->addr.sin_family = AF_INET; // IPv4
  inet_pton(AF_INET, server, &this->addr.sin_addr);
	this->addr.sin_port = htons(port);
  return this;
}


int
udp_client_send (udp_client_t* this, const char* buf, int len)
{
	return sendto (this->sockfd, buf, len, 0,
                 (const struct sockaddr *) &this->addr,
                 sizeof(this->addr));
}
