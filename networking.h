#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <errno.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <unistd.h>
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <crypt.h>

#ifndef NETWORKING_H
#define NETWORKING_H

#define BUFFER_SIZE 256
#define PORT "9003"
#define TEST_IP "127.0.0.1"
#define PACKET_SIZE 1024 //number of bytes in each file packet


void error_check(int i, char *s);
int num_non_null_bytes(char *s);
void wait_response(char * message, int server_socket);


int server_setup();
int server_connect(int sd);
int client_setup(char * server);

int forking_server();

void client(char *);

void handle_error();


#endif
