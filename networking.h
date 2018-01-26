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
#include <sys/ipc.h>
#include <sys/sem.h>

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
#define PORT "9001"
#define TEST_IP "0.0.0.0"
#define PACKET_SIZE 32768 //number of bytes that can be send between client and server for file transfers
#define LOGFILE_SIZE 2048 //number of bytes for users
#define ERROR_RESPONSE "-1"//start of every error message to client from server
#define ERROR_WAIT "-2" //server waits before printing full error

void error_check(int i, char *s);//defined in client.c
int num_non_null_bytes(char *s);//defined in client.c(returns number of non null bytes in s)
int wait_response(char * message, int socket);//defined in client.c(waits for a message from the client/server)


int server_setup();
int server_connect(int sd);
int client_setup(char * server);

int forking_server();//starts up forking server program

void client(char *serverIP);//starts up client program

void handle_error();//handles errno errors(defined in forking_server.c)


#endif
