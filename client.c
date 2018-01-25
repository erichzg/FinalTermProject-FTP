#include "networking.h"


int check_or_create_account(char * username, char * password, int server_socket, char * protocol);



//main client connection
void client(char * serverIP){
  char username[BUFFER_SIZE];
  char password[BUFFER_SIZE];
  int logged_in = -1;//0 once client is logged in

  int server_socket;
  char buffer[BUFFER_SIZE];
  char ans[BUFFER_SIZE];
  char file[BUFFER_SIZE];
  char filePath[BUFFER_SIZE];
  char fileContent[PACKET_SIZE];
  int fd;

  server_socket = client_setup(serverIP);


    //login code
    printf("Do you have an account yet?(y/n): ");
    memset(buffer, 0, sizeof(buffer));
    fgets(buffer, 256, stdin);
    *strchr(buffer, '\n') = 0;

    if(!strcmp(buffer,"y") || !strcmp(buffer, "Y")){
        while(logged_in < 0) {
            printf("Username: ");
            fgets(username, 256, stdin);
            *strchr(username, '\n') = 0;

            printf("Password: "); //make this hidden***
            fgets(password, 256, stdin);
            *strchr(password, '\n') = 0;

            logged_in = check_or_create_account(username, password,server_socket,"CHECK");
            if (logged_in < 0) {
                printf("Error logging in. Please try again\n");
            }
        }
    }
    else if(!strcmp(buffer, "n") || !strcmp(buffer, "N")){
        printf("Please no colons, semicolons, or pipes in your username or password.\n");
        while(logged_in < 0) {
            printf("Username: ");
            fgets(username, 256, stdin);
            *strchr(username, '\n') = 0;

            printf("Password: "); //make this hidden***
            fgets(password, 256, stdin);
            *strchr(password, '\n') = 0;

            logged_in = check_or_create_account(username, password,server_socket,"CREAT");
            if (logged_in < 0) {
                printf("Error creating account please try again\n");
            }
        }
    }
    else{
        printf("You did not type y or n. Try again next time.\n");
        close(server_socket);
        exit(0);
    }
    memset(password, 0, sizeof(password));

    printf("\nWelcome %s. You are now logged in",username);
    while (1) {
        printf("\nWould you like to push or pull (a file), \nview available files in the FTP, add collaborators, or exit? \n(push/pull/view/share/exit): ");
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

            if(!wait_response("2", server_socket)){//wait for confirmation to send file contents
                //file transfer
                printf("\nWhat is the path to this file?: ");
                fgets(filePath, sizeof(filePath), stdin);
                *strchr(filePath, '\n') = 0;
                //accessing file contents
                if ((fd = open(filePath, O_RDONLY)) < 0) //checks if file exists
                    handle_error();
                read(fd, fileContent, sizeof(fileContent));
                close(fd);
                //sending file contents up to NULL
                write(server_socket, fileContent, num_non_null_bytes(fileContent));
                printf("Pushed from '%s' to '%s'\n", filePath, file); //***
            }
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
            fd = open(filePath, O_CREAT|O_WRONLY|O_TRUNC, 0664);
            //writing into fd up to NULL
            write(fd, fileContent, num_non_null_bytes(fileContent));
            close(fd);

            printf("Pulled from '%s' to '%s'\n", file,filePath);

        }
        else if(!strcmp("view",buffer)){
            write(server_socket, "VIEW", sizeof("VIEW")); //view request sent

            //VIEWING CODE ***
            read(server_socket, stdout, sizeof(fileContent)); //get this to read to stdout***
        }
        else if(!strcmp("share",buffer)){
            //sending share request
            write(server_socket,"SHARE",sizeof("SHARE"));
            wait_response("1", server_socket);

            printf("\nWould you like to share push access?(y/n):");
            fgets(ans,sizeof(ans),stdin);
            *strchr(ans, '\n') = 0;

            if(!strcmp(ans,"y") || !strcmp(ans, "Y")){//push access sharing
                write(server_socket,"PUSH_SHARE",sizeof("PUSH_SHARE"));
                wait_response("2", server_socket);
            }
            else if(!strcmp(ans,"n") || !strcmp(ans, "N")){
                //pull file request***
            }

            //sending file name
            printf("\nWhat is the name of the file you are sharing?: ");
            fgets(file, sizeof(file), stdin);
            *strchr(file, '\n') = 0;
            write(server_socket, file, sizeof(file)); //file name sent

            if(!wait_response("3", server_socket)){ //waits for confirmation of share permissions
                //sending collaborator username
                printf("\nWho are you sharing it with?(enter a single username): ");
                fgets(ans, sizeof(ans), stdin);
                *strchr(ans, '\n') = 0;
                write(server_socket, ans, sizeof(ans)); //file name sent

                if(!wait_response("4", server_socket))//waits for file to be shared
                    printf("[%s] shared with %s\n",file,ans);
            }


        }
        else if(!strcmp("exit",buffer)) {
            printf("Thank you for using FTP. Goodbye\n");
            close(server_socket);
            exit(0);
        }
        else{
            printf("Please type in 'push', 'pull', 'view', 'share', or 'exit'.\n");
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

//returns 0 if everything checks out
//returns -1 if failure logging in
//protocol is either CHECK or CREAT
int check_or_create_account(char * username, char * password, int server_socket, char * protocol){

    //checking if passwords or usernames have colons or semicolons
    if(strchr(username, ':') || strchr(password, ':')
       || strchr(password, ';') || strchr(username, ';')
       || strchr(password, '|') || strchr(username, '|')){
        printf("Error: Username and password cannot contain colons, semicolons, or pipes.\n");
        return -1;
    }

    write(server_socket, protocol, sizeof(protocol)); //sends either CHECK or CREAT
    if(wait_response("1", server_socket) < 0) {
        return -1;
    }

    write(server_socket, username, sizeof(username)); //sending username
    if(wait_response("2", server_socket) < 0) {
        return -1;
    }

    char * encrypted = crypt(password, "ab");
    write(server_socket, encrypted, sizeof(encrypted)); //sending encrypted password
    if(wait_response("3", server_socket) < 0)
        return -1;

    return 0;
}

//waits until it receives expected message as response
//returns -1 if error occurs while waiting(error message sent)
//returns 0 upon success
int wait_response(char * message, int server_socket){
    char buffer[BUFFER_SIZE];
    while(strcmp(buffer,message)) {
        read(server_socket, buffer, sizeof(buffer));

        //if it gets error message instead of confirmation
        if(strstr(buffer,ERROR_RESPONSE)) {
            write(server_socket, ERROR_WAIT, sizeof(ERROR_WAIT));
            read(server_socket, buffer, sizeof(buffer)); //reading follow up error message
            printf("%s",buffer);
            return -1;
        }
    }
    return 0;
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