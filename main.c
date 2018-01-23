#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "networking.h"
#include "main.h"
#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <unistd.h>
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <crypt.h>



//returns 1 if everything cheks out
int checkuserinfo(char * username, char * password, char * serverIP){
    char * buffer = (char *) malloc(256 * sizeof(char));

    char * encrypted = crypt(password, "ab");
    //check file of encrypted passwords***
    int server_socket = hidden_connect_client(serverIP);

    write(server_socket, "CHECK", sizeof(buffer));
    write(server_socket, username, sizeof(buffer));
    write(server_socket, encrypted, sizeof(encrypted));
    read(server_socket, buffer, sizeof(buffer));
    return buffer[0] - '0';
}

//returns 1 if everything new account is created successfully
int create_account(char * username, char * password, char * serverIP){
    char * buffer = (char *) malloc(256 * sizeof(char));

    char * encrypted = crypt(password, "ab");
    //check file of encrypted passwords***
    int server_socket = hidden_connect_client(serverIP);

    write(server_socket, "CREATE", sizeof(buffer));
    write(server_socket, username, sizeof(buffer));
    write(server_socket, encrypted, sizeof(encrypted));
    read(server_socket, buffer, sizeof(buffer));
    return buffer[0] - '0';
}


int main(){
    //ask user if they wont to create a new server
    printf("Welcome to FTP\n Would you like to create a new server or use the original one?(y/n)\n");
    char * ans = (char *) malloc(256 * sizeof(char));
    char * username = (char *) malloc(256 * sizeof(char));
    char * password = (char *) malloc(256 * sizeof(char));
    char serverIP[256] = "100.2.206.108"; //DEFAULT IP FOR TESTING
    fgets(ans,256,stdin);
    
    if(ans[0] == 'y') {
        printf("Creating new server on this computer...\n");
        //creating new server base code***
        if(!fork()){ //child
            forking_server();
        }
    }

    int signedin = 0; //0 if not signed in
    int userId;

    printf("Do you have an account yet?(y/n)\n");
    memset(ans, 0, sizeof(ans));
    fgets(ans, 256, stdin);
    if(ans[0] == 'y'){
        while(!signedin) {
            printf("Username: \n");
            fgets(username, 256, stdin);
            printf("Password: \n"); //make this hidden***
            fgets(password, 256, stdin);
            signedin = checkuserinfo(username, password,serverIP);
            if (!signedin) {
                printf("Error logging in. Please try again\n");
            }
        }
    }
    else{
        while(!signedin) {
            printf("Please create a username: \n");
            fgets(username, 256, stdin);
            printf("Please type in a password: \n"); //make this hidden***
            fgets(password, 256, stdin);
            signedin = create_account(username, password,serverIP);
            if (!signedin) {
                printf("Username already exists please try again\n");
            }
        }
    }

    client(userId, serverIP);
    //free stuff***
    return 0;
}
