#include "networking.h"


int check_or_create_account(char * username, char * password, int server_socket, char * protocol);



//main client connection
void client(char * serverIP){
  char username[BUFFER_SIZE];
  char password[BUFFER_SIZE];
  int userId = 0;

  int server_socket;
  char buffer[BUFFER_SIZE];
  char file[BUFFER_SIZE];
  char filePath[BUFFER_SIZE];
  char fileContent[PACKET_SIZE];
  int fd;
  server_socket = client_setup(serverIP);


    //login code***
    printf("Do you have an account yet?(y/n)\n");
    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, 256, stdin);
    *strchr(buffer, '\n') = 0;
    if(!strcmp(buffer,"y") || !strcmp(buffer, "Y")){
        while(!userId) {
            printf("Username: \n");
            fgets(username, 256, stdin);
            *strchr(username, '\n') = 0;

            printf("Password: \n"); //make this hidden***
            fgets(password, 256, stdin);
            *strchr(password, '\n') = 0;

            userId = check_or_create_account(username, password,server_socket,"CHECK");
            if (!userId) {
                printf("Error logging in. Please try again\n");
            }
        }
    }
    else if(!strcmp(buffer, "n") || !strcmp(buffer, "Y")){
        while(!userId) {
            printf("Username: \n");
            fgets(username, 256, stdin);
            *strchr(username, '\n') = 0;

            printf("Password: \n"); //make this hidden***
            fgets(password, 256, stdin);
            *strchr(password, '\n') = 0;

            userId = check_or_create_account(username, password,server_socket,"CREAT");
            if (!userId) {
                printf("Username already exists please try again\n");
            }
        }
        memset(password, 0, sizeof(password));
        memset(username, 0, sizeof(username));
    }
    else{
        printf("You did not type y or n. Try again next time.\n");
        close(server_socket);
        exit(0);
    }

    while (1) {
        printf("\nWould you like to push or pull (a file), view available files in the FTP, or exit? (push/pull/view/exit): ");
        fgets(buffer, sizeof(buffer), stdin);
        *strchr(buffer, '\n') = 0;
        if(!strcmp("push",buffer)){ //push file code
            //sending push request
            write(server_socket, "PUSH", sizeof("PUSH")); //push request sent
            wait_response("1", server_socket);

            //sending file name
            printf("\nWhat is the name of the file you are pushing into?(if it doesnt exist yet one will be created): ");
            fgets(file, sizeof(file), stdin);
            *strchr(file, '\n') = 0;
            write(server_socket, file, sizeof(file)); //file name sent
            wait_response("2", server_socket);

            //file transfer
            printf("\nWhat is the path to this file?: ");
            fgets(filePath, sizeof(filePath), stdin);
            *strchr(filePath, '\n') = 0;
            //accessing file contents
            if((fd = open(filePath, O_RDONLY)) < 0) //checks if file exists
                handle_error();
            read(fd, fileContent, sizeof(fileContent));
            close(fd);
            //sending file contents up to NULL
            write(server_socket, fileContent, num_non_null_bytes(fileContent));

            printf("Pushed from '%s' to '%s'\n", filePath,file);

        }
        else if(!strcmp("pull",buffer)){//pull file code
            //sending pull request
            write(server_socket, "PULL", sizeof("PULL")); //pull request sent
            wait_response("1", server_socket);

            //sending file name
            printf("\nWhat is the name of the file you are pulling?: ");
            fgets(file, sizeof(file), stdin);
            *strchr(file, '\n') = 0;
            write(server_socket, file, sizeof(file)); //file name sent
            wait_response("2", server_socket);

            //receiving file contents
            write(server_socket, "3", sizeof("3"));//responds with a ready to read signal
            read(server_socket, fileContent, sizeof(fileContent));
            //storing file contents in client-side file
            printf("\nWhere would you like the file contents to be pulled?(enter a path to file): ");
            fgets(filePath, sizeof(filePath), stdin);
            *strchr(filePath, '\n') = 0;
            fd = open(filePath, O_CREAT|O_WRONLY|O_TRUNC);
            //writing into fd up to NULL
            write(fd, fileContent, num_non_null_bytes(fileContent));
            close(fd);

            printf("Pulled from '%s' to '%s'\n", file,filePath);

        }
        else if(!strcmp("view",buffer)) {
            write(server_socket, "VIEW", sizeof("VIEW")); //view request sent

            //VIEWING CODE ***
            read(server_socket, stdout, sizeof(fileContent)); //get this to read to stdout***
        }
        else if(!strcmp("exit",buffer)) {
            printf("Thank you for using FTP. Goodbye\n");
            close(server_socket);
            exit(0);
        }
        else{
            printf("Please type in 'push', 'pull', 'view', or 'exit'.");
        }
  }
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
    sleep(1);
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

//returns userID if everything checks out
//protocol is either CHECK or CREAT
int check_or_create_account(char * username, char * password, int server_socket, char * protocol){
    char * buffer = (char *) malloc(256 * sizeof(char));
    int retInt = 0;
    char * encrypted = crypt(password, "ab");
    //check file of encrypted passwords***

    write(server_socket, protocol, sizeof(protocol)); //sends either CHECK or CREAT
    wait_response("1", server_socket);

    write(server_socket, username, sizeof(username)); //sending username
    wait_response("2", server_socket);

    write(server_socket, encrypted, sizeof(encrypted)); //sending encrypted password*/
    wait_response("3", server_socket);

    read(server_socket, &retInt, sizeof(retInt)); //reading userId
    free(buffer);
    return retInt;
}

//waits until it recieves message as response
void wait_response(char * message, int server_socket){
    char * buffer = (char *) malloc(256 * sizeof(char));

    while(strcmp(buffer,message))
        read(server_socket, buffer, sizeof(buffer));
    free(buffer);
}

//returns number of bytes in s up to a NULL char
//i.e. the number of bytes of meaningful information
int num_non_null_bytes(char* s){
    char * nul_pos = strchr(s,'\0');
    //checks if NULL is actually in the string
    if(nul_pos != NULL)
        return sizeof(char)* (int)(nul_pos - s);
    else
        return sizeof(char)*PACKET_SIZE;
}