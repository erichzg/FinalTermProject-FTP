#include "networking.h"
#include "client.h"

void client(int userId){

  int server_socket;
  char buffer[BUFFER_SIZE];
  char * ans = (char *) malloc(256 * sizeof(char));


/*
  if (argc == 2)
    server_socket = client_setup( argv[1]);
  else
    server_socket = client_setup( TEST_IP );
*/



  while (1) {
    printf("enter data: ");
    fgets(buffer, sizeof(buffer), stdin);
    *strchr(buffer, '\n') = 0;
    write(server_socket, buffer, sizeof(buffer));
    read(server_socket, buffer, sizeof(buffer));
    printf("received: [%s]\n", buffer);
  }
}

void hidden_connect_client(){
    printf("w");
}
