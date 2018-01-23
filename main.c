#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "networking.h"
#include "main.h"


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
    //network setup stuff ***

    client(serverIP);
    return 0;
}
