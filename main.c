#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "networking.h"
#include "main.h"
#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <unistd.h>
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <crypt.h>



//returns userID if everything checks out
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

//returns userID if everything checks out
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
    char serverIP[256] = "127.0.0.1"; //DEFAULT IP FOR TESTING
    fgets(ans,256,stdin);
    *strchr(ans, '\n') = 0;
    if(ans[0] == 'y') {
        printf("Creating new server on this computer...\n");
        //creating new server base code***
        if(!fork()){ //child
            forking_server();
        }
    }

    int userId = 0;

    /*printf("Do you have an account yet?(y/n)\n");
    memset(ans, 0, sizeof(ans));
    fgets(ans, 256, stdin);
    if(ans[0] == 'y'){
        while(!userId) {
            printf("Username: \n");
            fgets(username, 256, stdin);
            printf("Password: \n"); //make this hidden***
            fgets(password, 256, stdin);
            userId = checkuserinfo(username, password,serverIP);
            if (!userId) {
                printf("Error logging in. Please try again\n");
            }
        }
    }
    else{
        while(!userId) {
            printf("Please create a username: \n");
            fgets(username, 256, stdin);
            printf("Please type in a password: \n"); //make this hidden***
            fgets(password, 256, stdin);
            userId = create_account(username, password,serverIP);
            if (!userId) {
                printf("Username already exists please try again\n");
            }
        }
    }*/

    client(userId, serverIP);
    //free stuff***
    return 0;
}
