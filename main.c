#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

int main(){
    //ask user if they wont to create a new server
    printf("Welcome to FTP\n Would you like to create a new server or use the original one?(y/n)\n");
    char ans[256];
    fgets(&ans,256,stdin);
    if(!strcmp(ans,"y")){
        printf("Creating new server...\n");
    }
    else{
        printf("Starting up client program...\n");
        execvp("client.o","client.o",NULL);
    }
    return 0;
}