#include "networking.h"

void process(char *s);
void subserver(int from_client);
void print_packet(char *s);

void handle_error(){
    printf("Error: %s\n", strerror(errno));
    exit(1);
}
void print_packet(char *s){
    printf("[Server]: received [%s]\n", s);
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
    char fileContent[PACKET_SIZE];
    char filesInDir[PACKET_SIZE];
    //concerning login
    char username[BUFFER_SIZE];
    char enc_password[BUFFER_SIZE];
    char * double_enc; // double encrypted password
    char account_desc[PACKET_SIZE]; //contains both password and username + wiggle room
    char accounts_content[LOGFILE_SIZE];
    int fd;
    int logged_in = -1;//0 if user is logged in

  while (read(client_socket, buffer, sizeof(buffer))) {

    if(!strcmp(buffer, "PUSH") && !logged_in){ //dealing with push request
        write(client_socket, "1", sizeof("1")); //responds to client

        read(client_socket, file, sizeof(file)); //receives file name
        print_packet(file);
        write(client_socket, "2", sizeof("2")); //responds

        //file transfer code ***
        strcpy(filePath, "./fileDir/");
        strcat(filePath,file);
        fd = open(filePath, O_CREAT|O_WRONLY|O_TRUNC);
        //receiving file contents
        read(client_socket, fileContent, sizeof(fileContent));
        print_packet(fileContent);
        //writing into fd up to NULL
        write(fd, fileContent, num_non_null_bytes(fileContent));
        close(fd);
        printf("[Server]: pushed to '%s'\n", file);

    }
    else if(!strcmp(buffer,"PULL") && !logged_in){ //dealing with a pull request
        write(client_socket, "1", sizeof("1")); //responds to client

        read(client_socket, file, sizeof(file)); //receives file name
        print_packet(file);
        write(client_socket, "2", sizeof("2")); //responds

        //file transfer code ***
        strcpy(filePath, "./fileDir/");
        strcat(filePath,file);
        //accessing file contents
        if((fd = open(filePath, O_RDONLY)) < 0) //checks if file exists
            handle_error();
        read(fd, fileContent, sizeof(fileContent));

        //sending file contents up to NULL
        wait_response("3", client_socket);
        write(client_socket, fileContent, num_non_null_bytes(fileContent));
        close(fd);
        printf("[Server]: pulled from '%s'\n", file);

    }
    else if(!strcmp(buffer,"VIEW") && !logged_in){
        //VIEWING CODE ***
        write(client_socket, filesInDir, sizeof(fileContent));
    }
    else if(!strcmp(buffer,"CHECK")){

        //accessing username/ encrypted password
        write(client_socket, "1", sizeof("1"));
        read(client_socket, username, sizeof(username));
        print_packet(username);

        write(client_socket, "2", sizeof("2"));
        read(client_socket, enc_password, sizeof(enc_password));
        double_enc = crypt(enc_password, "bc");


        //ACCOUNT CHECKING CODE ***

        //sends back final confirmation
        write(client_socket, "3", sizeof("3"));
    }
    else if(!strcmp(buffer,"CREAT")){

        //accessing username/ encrypted password
        write(client_socket, "1", sizeof("1"));
        read(client_socket, username, sizeof(username));
        print_packet(username);

        write(client_socket, "2", sizeof("2"));
        read(client_socket, enc_password, sizeof(enc_password));
        double_enc = crypt(enc_password, "bc");
        strcat(username, ":");

        //opens passwords file (creates it when first user creates account)
        fd = open("./accounts.txt", O_RDWR|O_CREAT|O_APPEND, 0644);
        read(fd, accounts_content, sizeof(accounts_content));
        //checks if username already exists(with colon at end to only look for usernames in accounts file)
        if(strstr(accounts_content, username)){
            printf("[Server]: Error username already exist\n");
            write(client_socket, ERROR_RESPONSE, sizeof(ERROR_RESPONSE));
            write(client_socket, "ERROR: Username already exists\n", sizeof("ERROR: Username already exists\n"));
            close(fd);
        }
        else {
            sprintf(account_desc, "%s%s\n%c", username, double_enc, '\0'); //username already has colon at end
            write(fd, account_desc, num_non_null_bytes(account_desc));
            close(fd);

            //sends back final confirmation and "logs in" client
            logged_in = 0;
            write(client_socket, "3", sizeof("3"));
        }
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