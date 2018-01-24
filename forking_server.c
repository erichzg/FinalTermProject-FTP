#include "networking.h"

void process(char *s);
void subserver(int from_client);
void print_packet(char *s);

void handle_error(){
    printf("Error: %s\n", strerror(errno));
    exit(1);
}
void print_packet(char *s){
    printf("[Server]: received %s\n", s);
}

int forking_server() {

  int listen_socket;
  int f;
  listen_socket = server_setup();

  while (1) {

    int client_socket = server_connect(listen_socket);
    f = fork();
    if (f == 0)
      subserver(client_socket);
    // does parent need to close socket? ***  
  }
}

void subserver(int client_socket) {
  char buffer[BUFFER_SIZE];
    char file[BUFFER_SIZE];
    char filePath[BUFFER_SIZE];
    char fileContent[1024];
    char filesInDir[1024];
    int fd;

  while (read(client_socket, buffer, sizeof(buffer))) {

    if(!strcmp(buffer, "PUSH")){ //dealing with push request
        write(client_socket, buffer, sizeof(buffer)); //responds to client
        read(client_socket, file, sizeof(file)); //recieves file name
        print_packet(file);
        write(client_socket, buffer, sizeof(buffer)); //responds

        //file transfer code ***
        strcpy(filePath, "./fileDir/");
        strcat(filePath,file);
        fd = open(filePath, O_CREAT|O_WRONLY);
        //recieving file contents
        read(client_socket, fileContent, sizeof(fileContent));
        print_packet(fileContent);
        //writing into fd
        write(fd, fileContent, sizeof(fileContent));
        close(fd);
    }
    else if(!strcmp(buffer,"PULL")){ //dealing with a pull request
        write(client_socket, buffer, sizeof(buffer)); //responds to client
        read(client_socket, file, sizeof(file)); //recieves file name
        print_packet(file);
        write(client_socket, buffer, sizeof(buffer)); //responds

        //file transfer code ***
        strcpy(filePath, "./fileDir/");
        strcat(filePath,file);
        //accessing file contents
        if((fd = open(filePath, O_RDONLY)) < 0) //checks if file exists
            handle_error();
        read(fd, fileContent, sizeof(fileContent));
        print_packet(fileContent);
        //sending file contents
        write(client_socket, fileContent, sizeof(fileContent));
        close(fd);
    }
    else if(!strcmp(buffer,"VIEW")){
        //VIEWING CODE ***
        write(client_socket, filesInDir, sizeof(fileContent));
    }
    else if(!strcmp(buffer,"CHECK")){
        //PASSWORD CHECKING CODE ***
        int userId = 1;
        write(client_socket, &userId, sizeof(userId));
    }
    else if(!strcmp(buffer,"CREATE")){
        //ACCOUNT CREATING CODE ***
        int userId = 1;
        write(client_socket, &userId, sizeof(userId));
    }
  }//end read loop
  close(client_socket);
  exit(0);
}

void process(char * s) {
  while (*s) {
    if (*s >= 'a' && *s <= 'z')
      *s = ((*s - 'a') + 13) % 26 + 'a';
    else  if (*s >= 'A' && *s <= 'Z')
      *s = ((*s - 'a') + 13) % 26 + 'a';
    s++;
  }
}


/*=========================
  server_setup
  args:

  creates, binds a server side socket
  and sets it to the listening state

  returns the socket descriptor
  =========================*/
int server_setup() {
    int sd, i;

    //create the socket
    sd = socket( AF_INET, SOCK_STREAM, 0 );
    error_check( sd, "server socket" );
    printf("[server] socket created\n");

    //setup structs for getaddrinfo
    struct addrinfo * hints, * results;
    hints = (struct addrinfo *)calloc(1, sizeof(struct addrinfo));
    hints->ai_family = AF_INET;  //IPv4 address
    hints->ai_socktype = SOCK_STREAM;  //TCP socket
    hints->ai_flags = AI_PASSIVE;  //Use all valid addresses
    getaddrinfo(TEST_IP, PORT, hints, &results); //NULL means use local address***

    //bind the socket to address and port
    i = bind( sd, results->ai_addr, results->ai_addrlen );
    error_check( i, "server bind" );
    printf("[server] socket bound\n");

    //set socket to listen state
    i = listen(sd, 10);
    error_check( i, "server listen" );
    printf("[server] socket in listen state\n");

    //free the structs used by getaddrinfo
    free(hints);
    freeaddrinfo(results);
    return sd;
}


/*=========================
  server_connect
  args: int sd

  sd should refer to a socket in the listening state
  run the accept call

  returns the socket descriptor for the new socket connected
  to the client.
  =========================*/
int server_connect(int sd) {
    int client_socket;
    socklen_t sock_size;
    struct sockaddr_in client_address;

    // client_socket = accept(sd, (struct sockaddr *)&client_address, &sock_size);

    sock_size = sizeof(struct sockaddr_in);

    //accept connection from an incoming client
    client_socket = accept(sd, (struct sockaddr *)&client_address, &sock_size);

    error_check(client_socket, "server accept");

    if(client_socket > 0){
        printf("[server] connection established: client_socket[%d]\n", client_socket);
    }
    return client_socket;
}