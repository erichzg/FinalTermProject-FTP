#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "networking.h"


int main() {
    char serverIP[256]; //DEFAULT IP FOR TESTING

    //ask user if they wont to create a new server or client
    printf("\nWelcome to FTP\nPress c to run client or s to run server.\n(A server must be running for client to work)\n");
    char * ans = (char *) malloc(256 * sizeof(char));
    fgets(ans, 256, stdin);
    *strchr(ans, '\n') = 0;

    if(ans[0] == 's' || ans[0] == 'S') {
        printf("Creating new server on this computer...\n");
        forking_server();
    }
        else if(ans[0] == 'c' || ans[0] == 'C') {
            printf("What server IP do you want to connect to?\n(0.0.0.0 if there the server on this computer)\n");
            fgets(ans, 256, stdin);
            *strchr(ans, '\n') = 0;
            client(ans);
        } else {
            printf ("Cannot not do %s. try c or s next time.\n", ans);
        }

    //network setup stuff ***
    return 0;
}
