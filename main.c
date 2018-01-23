#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "client.h"
#include "main.h"
#define _XOPEN_SOURCE       /* See feature_test_macros(7) */
#include <unistd.h>
#define _GNU_SOURCE         /* See feature_test_macros(7) */
#include <crypt.h>



//returns 1 if everything cheks out
int checkuserinfo(char * username, char * password){
    char * encrypted = crypt(password, "ab");
    //check file of encrypted passwords***
    hidden_connect_client();
    return 0;
}


int main(){
    //ask user if they wont to create a new server
    printf("Welcome to FTP\n Would you like to create a new server or use the original one?(y/n)\n");
    char * ans = (char *) malloc(256 * sizeof(char));
    char * username = (char *) malloc(256 * sizeof(char));
    char * password = (char *) malloc(256 * sizeof(char));
    fgets(ans,256,stdin);
    
    if(ans[0] == 'y') {
        printf("Creating new server...\n");
        //creating new server base code***
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
            signedin = checkuserinfo(username, password);
            if (!signedin) {
                printf("Error logging in. Please try again\n");
            }
        }
    }

    client(userId);
    //free stuff***
    return 0;
}
