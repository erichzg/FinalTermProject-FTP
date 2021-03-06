#include "networking.h"

void subserver(int from_client);
void print_packet(char *s);

/*=========================
  view_files_into
  args: char * user, char * perm_file

  finds all files user has access too in perm_file

  returns pointer to string containing accessible files
  =========================*/
char * view_files_into(char * user, char * perm_file) {
  int fd = open(perm_file, O_RDONLY, 0644);
  char fileContent[LOGFILE_SIZE];
  memset(fileContent, 0, sizeof(fileContent));
  read(fd, fileContent, sizeof(fileContent));
  close(fd);
  
  const char s[2] = "|";
  char * token;
  char file[256];
  memset(file, 0, sizeof(file));
  char * correct_files;
  correct_files = (char *) calloc(LOGFILE_SIZE, sizeof(char));
   
  // get the first token
  token = strtok(fileContent, s);

  // walk through other tokens
  while( token != NULL ) {
    if(strstr(token,user)){
        memset(file, 0, sizeof(file));
        strncpy(file, token, sizeof(char) * (int)(strchr(token, ';')-token));
        strcpy(correct_files + strlen(correct_files), file);
      strcpy(correct_files + strlen(correct_files), ", ");
    }
    token = strtok(NULL, s);
  }
  if (!(correct_files[0])) {
    return "\n";
  }
  return correct_files;
}


/*=========================
  hash
  args: unsigned char *str

  djb2 from FOSS. hashes str into int(for creating unique semaphore KEY's)

  returns int value from hashing
  =========================*/
unsigned int hash(unsigned char *str) {
  unsigned int hash = 5381;
  int c;
  while (c = *str++)
     hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
  return hash;
}

/*=========================
  handle_error

  args:

  Prints error in errno if there is one
  =========================*/
void handle_error(){
    printf("Error: %s\n", strerror(errno));
    exit(1);
}
/*=========================
  print_packet

  args: s

  Prints packet(s) that server recieved(not used with uppercase requests)
  =========================*/
void print_packet(char *s){
    printf("[Server]: received [%s]\n", s);
}

/*=========================
  forking_server

  args:

  starts up forking server and forks off subserver whenever a client connects
  =========================*/
int forking_server() {

  int listen_socket;
  int f;
  listen_socket = server_setup();

  while (1) {

    int client_socket = server_connect(listen_socket);
    f = fork();
    if (f == 0)
      subserver(client_socket);
  }
}

/*=========================
  subserver
  args: int client_socket

  subserver code for handling client through client_socket
  deals with client requests, account, permission, and file handling
  =========================*/
void subserver(int client_socket) {
    char temp_buffer[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    //concerning file transfering and permissions
    char file[BUFFER_SIZE];//file name
    char filePath[BUFFER_SIZE];//file path
    char fileContent[PACKET_SIZE];//transfers up to 32KB of file content
    char perm_desc[LOGFILE_SIZE]; //<file>;<username1>:<username2>:
    char perms_content[LOGFILE_SIZE]; //all permissions
    char *init_file_pos; //pointer to where a file permissions begin

    //concerning file sharing
    char collab_username[BUFFER_SIZE];//username of collaborator

    //concerning login
    char username[BUFFER_SIZE]; //set to username + : at login stage and used for verifying permissions while logged in
    char enc_password[BUFFER_SIZE]; //ab salt crypt from client
    char * double_enc; // bc salt crypt on ab salt crypt from client
    char expected_password[BUFFER_SIZE]; //also double encrypted
    char *init_username_pos; //pointer to where username in account file begins
    char account_desc[LOGFILE_SIZE]; //<username>:<password>;
    char accounts_content[LOGFILE_SIZE]; //content of file with usernames and passwords
    int logged_in = -1;//0 if user is logged in

  while (read(client_socket, buffer, sizeof(buffer))) {
    /*-----------------
        PUSH request

        Checks if requested file exists in database
        If file hasn't been created, then create one and append permissions for owner into push_perm.txt and pull_perm.txt
        (and creates a semaphore KEY for that file, setting its value to 1)
        If file already exists verify users push permissions and sets semaphore to 1 if all checks out

        If push request is granted than send confirmation to client and read file data from
          client_socket into file in database
    -----------------*/
    if(!strcmp(buffer, "PUSH") && !logged_in){ //dealing with push request
        write(client_socket, "1", sizeof("1")); //responds to client

        read(client_socket, file, sizeof(file)); //receives file name
        print_packet(file);

        //file transfer code
        strcpy(filePath, "./fileDir/");
        strcat(filePath,file);
        strcat(file, ";"); //adds ; to the end of file name
        int fd;//of file being pushed into(initialized without permission to push)
        int push_perm_fd = open("./push_perm.txt", O_CREAT|O_RDWR|O_APPEND, 0664); //to work with push permissions

        if((fd = open(filePath, O_CREAT|O_EXCL|O_WRONLY, 0664))>=0){//if file doesn't exist yet
            sprintf(perm_desc, "%s%s|%c",file,username,'\0'); //username/file already have correct endings
            int pull_perm_fd = open("./pull_perm.txt", O_CREAT|O_RDWR|O_APPEND, 0664); //to add pull permissions as well
            write(push_perm_fd, perm_desc, num_non_null_bytes(perm_desc));//writes new permission at end
            write(pull_perm_fd, perm_desc, num_non_null_bytes(perm_desc));//writes new permission at end
 
            int sd = semget(hash(file), 1, 0644 | IPC_CREAT | IPC_EXCL);//make a semaphore
            semctl(sd, 0, SETVAL, 1);//set value to 1

            write(client_socket,"2",sizeof("2"));//confirms push access
        }
        else{//file exists (push permissions need to be verified)
            read(push_perm_fd, perms_content, sizeof(perms_content));
            init_file_pos = strstr(perms_content,file); //guaranteed to be file because of ; ending
            strncpy(perm_desc,                      //copy into description of permission...
                    init_file_pos,     //starting from the beginning of description...
                    sizeof(char)*(int)(strchr(init_file_pos,'|')-init_file_pos)); //to the end of the description
            if(strstr(perm_desc,username)){//if username found in permissions
                fd = open(filePath, O_WRONLY|O_TRUNC, 0664);

                int sd = semget(hash(file), 1, 0644);
                struct sembuf temp_sembuf;
                temp_sembuf.sem_op = 1;
                temp_sembuf.sem_num = 0;
                semctl(sd, 0, SETVAL, 1);//resets value to one(doesn't UP since multiple pushes would make val>1)

                write(client_socket,"2",sizeof("2"));//confirms push access
            }
            else{//push access denied
                fd = -1;//prevent operations on any file
                write(client_socket,ERROR_RESPONSE,sizeof(ERROR_RESPONSE));
                wait_response(ERROR_WAIT,client_socket);
                write(client_socket,"ERROR: Push access denied\n",sizeof("ERROR: Push access denied\n"));
            }
        }
        close(push_perm_fd);

        if(fd >= 0){//if server decides to push
            //receiving file contents
            memset(fileContent,0,sizeof(fileContent));
            read(client_socket, fileContent, sizeof(fileContent));
            print_packet(fileContent);
            //writing into fd up to NULL
            write(fd, fileContent, num_non_null_bytes(fileContent));
            close(fd);
            printf("[Server]: pushed to '%s'\n", file);
        }
    }
    /*-----------------
        PULL request

        Checks if requested file exists
        Checks if client has pull permissions to file(downs file's semaphore by 1 if all checks out)

        If pull request is granted than send confirmation to client and send file data to client_socket
    -----------------*/
    else if(!strcmp(buffer,"PULL") && !logged_in){ //dealing with a pull request
        write(client_socket, "1", sizeof("1")); //responds to client

        read(client_socket, file, sizeof(file)); //receives file name
        print_packet(file);

        strcpy(filePath, "./fileDir/");
        strcat(filePath,file);
        strcat(file, ";"); //adds ; to the end of file name

        int fd = -1;
        int pull_perm_fd = open("./pull_perm.txt", O_RDONLY, 0664); //to work with pull permissions
        read(pull_perm_fd, perms_content, sizeof(perms_content));
        init_file_pos = strstr(perms_content,file); //guaranteed to be file because of ; ending
        if(!init_file_pos){//if file not found
            write(client_socket,ERROR_RESPONSE,sizeof(ERROR_RESPONSE));
            wait_response(ERROR_WAIT,client_socket);
            write(client_socket,"ERROR: File not found\n",sizeof("ERROR: File not found\n"));
        }
        else{//if file found(check pull permissions)
            strncpy(perm_desc,                      //copy into description of permission...
                    init_file_pos,     //starting from the beginning of description...
                    sizeof(char)*(int)(strchr(init_file_pos,'|')-init_file_pos)); //to the end of the description

            if(strstr(perm_desc,username)){//if username found in permissions
                fd = open(filePath, O_RDONLY, 0664);

                int sd = semget(hash(file), 1, 0644);
                struct sembuf temp_sembuf;
                temp_sembuf.sem_op = -1;
                temp_sembuf.sem_num = 0;
                temp_sembuf.sem_flg = IPC_NOWAIT | SEM_UNDO;
                int temp = semop(sd, &temp_sembuf, 1);

                if (temp < 0) { //if someone has already pulled(and hasn't repushed)
                    write(client_socket,ERROR_RESPONSE,sizeof(ERROR_RESPONSE));
                    wait_response(ERROR_WAIT,client_socket);
                    write(client_socket,"File currently in use\n", sizeof("File currently in use\n"));
                    fd = -1;
                }
                else //confirm pull access
                    write(client_socket,"2",sizeof("2"));
            }
            else{//pull access denied
                fd = -1;
                write(client_socket,ERROR_RESPONSE,sizeof(ERROR_RESPONSE));
                wait_response(ERROR_WAIT,client_socket);
                write(client_socket,"ERROR: Pull access denied\n",sizeof("ERROR: Pull access denied\n"));
            }
        }

        if(fd >= 0){//if server decides to pull
            //accessing file contents
            memset(fileContent,0,sizeof(fileContent));
            read(fd, fileContent, sizeof(fileContent));
            //sending file contents up to NULL
            wait_response("3", client_socket);
            write(client_socket, fileContent, num_non_null_bytes(fileContent));
            close(fd);
            printf("[Server]: pulled from '%s'\n", file);
        }
        close(pull_perm_fd);
    }
    /*-----------------
        VIEW request

        Goes through pull_perm.txt and push_perm.txt and sends client the files
         that they have access to
    -----------------*/
    else if(!strcmp(buffer,"VIEW") && !logged_in){
      char * pull_files;
      pull_files = view_files_into(username, "pull_perm.txt");
      write(client_socket, pull_files, strlen(pull_files));

      wait_response("1",client_socket);
      char * push_files;
      push_files = view_files_into(username, "push_perm.txt");
      write(client_socket, push_files, strlen(push_files));
    }
    /*-----------------
        SHARE request

        Handles both push and pull sharing
        Checks if requested file to share exists
        Checks if client has permission to push(pull) and allows client to share
         that permission if they do
        Appends username to share to into push_perm.txt(pull_perm.txt)
    -----------------*/
    else if(!strcmp(buffer,"SHARE") && !logged_in){
        write(client_socket, "1", sizeof("1")); //responds to client

        read(client_socket, temp_buffer, sizeof(temp_buffer)); //receives type of share
        print_packet(temp_buffer);
        write(client_socket, "2", sizeof("2")); //responds

        read(client_socket, file, sizeof(file)); //receives file name
        print_packet(file);

        //verifying permissions to file
        int perm_fd = open(!strcmp(temp_buffer,"PUSH_SHARE") ? "./push_perm.txt":"./pull_perm.txt",
                           O_RDONLY, 0664); //to work with file permissions
        read(perm_fd, perms_content, sizeof(perms_content));
        strcat(file,";");
        init_file_pos = strstr(perms_content,file); //guaranteed to be correct file because of ; ending

        if(!init_file_pos){//if file not found
            write(client_socket,ERROR_RESPONSE,sizeof(ERROR_RESPONSE));
            wait_response(ERROR_WAIT,client_socket);
            write(client_socket,"ERROR: File not found\n",sizeof("ERROR: File not found\n"));
        }
        else {//if file found(checking permissions)
            strncpy(perm_desc,                      //copy into description of permission...
                    init_file_pos,     //starting from the beginning of description...
                    sizeof(char) * (int) (strchr(init_file_pos, '|') - init_file_pos)); //to the end of the description

            if (!strstr(perm_desc, username)) {//share access denied
                write(client_socket, ERROR_RESPONSE, sizeof(ERROR_RESPONSE));
                wait_response(ERROR_WAIT, client_socket);
                write(client_socket, "ERROR: Share access denied\n", sizeof("ERROR: Share access denied\n"));
            }
            else {//if username found in permissions
                write(client_socket, "3", sizeof("3")); //confirms permission to share

                //receives person to share with
                read(client_socket, collab_username, sizeof(collab_username));
                print_packet(collab_username);
                if(strchr(collab_username, ':') || strchr(collab_username, '|')|| strchr(collab_username, ';')){
                    write(client_socket, ERROR_RESPONSE, sizeof(ERROR_RESPONSE));
                    wait_response(ERROR_WAIT, client_socket);
                    write(client_socket, "ERROR: Username contains '|',':', or ';'\n",
                          sizeof("ERROR: Username contains '|',':', or ';'\n"));
                }
                else{//if username doesn't have illegal chars
                    strcat(collab_username,":");

                    //reopening file for overwriting purposes
                    close(perm_fd);
                    perm_fd = open(!strcmp(temp_buffer,"PUSH_SHARE") ? "./push_perm.txt":"./pull_perm.txt",
                                   O_WRONLY|O_TRUNC, 0664); //to work with push permissions
                    //enters head of content(up to where new permission will be added)
                    write(perm_fd,perms_content,sizeof(char)*(int)(strchr(init_file_pos,';')+1-perms_content));
                    //enters new permissions
                    write(perm_fd,collab_username,num_non_null_bytes(collab_username));
                    //enters tail of content
                    write(perm_fd,strchr(init_file_pos,';')+1,num_non_null_bytes(strchr(init_file_pos,';')+1));

                    write(client_socket, "4", sizeof("4")); //confirms sharing
                }
            }
        }
        close(perm_fd);
    }
    /*-----------------
        CHECK request

        For signing in
        Checks if username is in accounts.txt
        Encrypts the encrypted password(2nd time) and checks if it matches the one in accounts.txt
        Sets logged_in to 0 if all checks out and keeps username+: for future requests
    -----------------*/
    else if(!strcmp(buffer,"CHECK")){

        //accessing username/ encrypted password
        write(client_socket, "1", sizeof("1"));
        read(client_socket, username, sizeof(username));
        print_packet(username);
        write(client_socket, "2", sizeof("2"));
        read(client_socket, enc_password, sizeof(enc_password));

        //opens passwords file
        int fd = open("./accounts.txt", O_RDONLY, 0644);
        read(fd, accounts_content, sizeof(accounts_content));

        //checks if username/password are correct
        double_enc = crypt(enc_password, "bc");
        strcat(username, ":");
        init_username_pos = strstr(accounts_content, username);//where the username + : is found

        if(init_username_pos){ //if the username was found
            strncpy(expected_password,                      //copy into expected password...
                    strchr(init_username_pos,':') + 1,     //starting from the beginning of the password...
                    sizeof(char)*(int)(strchr(init_username_pos,';')-strchr(init_username_pos,':')-1));//length of password
            if(!strcmp(double_enc,expected_password)) {//if passwords match
                //sends back final confirmation and "log in" client
                logged_in = 0;
                write(client_socket, "3", sizeof("3"));
            }
            else{//if password doesn't match
                printf("[Server]: Error username or password isn't correct\n");
                write(client_socket, ERROR_RESPONSE, sizeof(ERROR_RESPONSE));
                wait_response(ERROR_WAIT,client_socket);
                write(client_socket, "ERROR: Username or password isn't correct\n", sizeof("ERROR: Username or password isn't correct\n"));
                close(fd);
            }
        }
        else{ //if username not found in accounts_content
            printf("[Server]: Error username doesn't exist\n");
            write(client_socket, ERROR_RESPONSE, sizeof(ERROR_RESPONSE));
            wait_response(ERROR_WAIT,client_socket);
            write(client_socket, "ERROR: Username doesn't exist\n", sizeof("ERROR: Username doesn't exist\n"));
            close(fd);

        }
    }
    /*-----------------
        CREAT request

        For creating account
        Checks if the username already exists in accounts.txt(it shouldn't)
        Encrypts the encrypted password(2nd time) and appends it with username into accounts.txt
        Sets logged_in to 0 if all checks out and keeps username+: for future requests
    -----------------*/
    else if(!strcmp(buffer,"CREAT")){

        //accessing username/ encrypted password
        write(client_socket, "1", sizeof("1"));
        read(client_socket, username, sizeof(username));
        print_packet(username);

        write(client_socket, "2", sizeof("2"));
        read(client_socket, enc_password, sizeof(enc_password));

        //opens passwords file (creates it when first user creates account)
        int fd = open("./accounts.txt", O_RDWR|O_CREAT|O_APPEND, 0644);
        read(fd, accounts_content, sizeof(accounts_content));
        //checks if username already exists(with colon at end to only look for usernames in accounts file)
        strcat(username, ":");
        if(strstr(accounts_content, username)){
            printf("[Server]: Error username already exist\n");
            write(client_socket, ERROR_RESPONSE, sizeof(ERROR_RESPONSE));
            wait_response(ERROR_WAIT,client_socket);
            write(client_socket, "ERROR: Username already exists\n", sizeof("ERROR: Username already exists\n"));
            close(fd);
        }
        else {
            //combines strings and puts a ;NULL at the end (; separate multiple accounts)
            double_enc = crypt(enc_password, "bc");
            sprintf(account_desc, "%s%s;%c", username, double_enc, '\0'); //username already has colon at end
            write(fd, account_desc, num_non_null_bytes(account_desc));
            close(fd);

            //sends back final confirmation and "logs in" client
            logged_in = 0;
            write(client_socket, "3", sizeof("3"));
        }
    }
    else{//doesn't recieve one of known commands
        write(client_socket, ERROR_RESPONSE, sizeof(ERROR_RESPONSE));
        wait_response(ERROR_WAIT,client_socket);
        write(client_socket, "ERROR: Didn't understand command please try again\n",
              sizeof("ERROR: Didn't understand command please try again\n"));
    }
  }//end read loop
  close(client_socket);
  exit(0);
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
    getaddrinfo(NULL, PORT, hints, &results); //NULL means use local address

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
