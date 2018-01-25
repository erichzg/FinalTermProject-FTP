#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "networking.h"


int main() {
    char serverIP[256]; //DEFAULT IP FOR TESTING

    //ask user if they wont to create a new server or client
    printf("Welcome to FTP\nPress c to run client or s to run server.\n");
    char * ans = (char *) malloc(256 * sizeof(char));
    fgets(ans, 256, stdin);
    *strchr(ans, '\n') = 0;

    if(ans[0] == 's' || ans[0] == 'S') {
        printf("Creating new server on this computer...\n");
        //creating new server base code**

        forking_server();
    }
        else if(ans[0] == 'c' || ans[0] == 'C') {
            printf("What server IP do you want to connect to?\n");
            fgets(ans, 256, stdin);
            *strchr(ans, '\n') = 0;
            client(ans);
        } else {
            printf ("Cannot not do %s. try c or s next time.\n", ans);
        }

    //network setup stuff ***
    return 0;
}
