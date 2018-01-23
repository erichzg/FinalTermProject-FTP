#include "networking.h"


//main client connection
void client(int userId, char * serverIP){

  int server_socket;
  char buffer[BUFFER_SIZE];
  char * ans = (char *) malloc(256 * sizeof(char));
  server_socket = client_setup( serverIP);



    while (1) {
    printf("enter data: ");
    fgets(buffer, sizeof(buffer), stdin);
    *strchr(buffer, '\n') = 0;
    write(server_socket, buffer, sizeof(buffer));
    read(server_socket, buffer, sizeof(buffer));
    printf("received: [%s]\n", buffer);
  }
}

int hidden_connect_client(char * serverIP){
    int server_socket;
    server_socket = client_setup( serverIP);
    return server_socket;
}

/*=========================
  client_setup
  args: int * to_server

  to_server is a string representing the server address

  create and connect a socket to a server socket that is
  in the listening state

  returns the file descriptor for the socket
  =========================*/
int client_setup(char * server) {
    int sd, i;

    //create the socket
    sd = socket( AF_INET, SOCK_STREAM, 0 );
    error_check( sd, "client socket" );

    //run getaddrinfo
    /* hints->ai_flags not needed because the client
       specifies the desired address. */
    struct addrinfo * hints, * results;
    hints = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
    hints->ai_family = AF_INET;  //IPv4
    hints->ai_socktype = SOCK_STREAM;  //TCP socket
    getaddrinfo(server, PORT, hints, &results);

    //connect to the server
    //connect will bind the socket for us
    i = connect( sd, results->ai_addr, results->ai_addrlen );
    error_check( i, "client connect" );

    free(hints);
    freeaddrinfo(results);

    return sd;
}

void error_check( int i, char *s ) {
    if ( i < 0 ) {
        printf("[%s] error %d: %s\n", s, errno, strerror(errno) );
        exit(1);
    }
}