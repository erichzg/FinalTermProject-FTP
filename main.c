#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "client.h"
#include "main.h"

//returns 1 if everything cheks out
int checkuserinfo(char * username, char * password){
    return 0;
}


int main(){
    //ask user if they wont to create a new server
    printf("Welcome to FTP\n Would you like to create a new server or use the original one?(y/n)\n");
    char * ans = (char *) malloc(256 * sizeof(char));
    char * username = (char *) malloc(256 * sizeof(char));
    char * password = (char *) malloc(256 * sizeof(char));
    fgets(ans,256,stdin);
    
    if(!strcmp(ans,"y")) {
        printf("Creating new server...\n");
        //creating new server base code***
    }

    int signedin = 0; //0 if not signed in
    int userId;

    printf("Do you have an account yet?(y/n)\n");
    fgets(ans, 256, stdin);
    if(!strcmp(ans,"y")){
        printf("Username: \n");
        fgets(username, 256, stdin);
        printf("Password: \n"); //make this hidden***
        fgets(password, 256, stdin);
        if(!checkuserinfo(username, password)){
            printf("error logging in\n");
        }
    }

    client(userId);
    //free stuff***
    return 0;
}