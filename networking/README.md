# Networking examples

## Files
* netowrking.h/.c
  * Code to set up socket connections for both server and client programs.
* client.c
  * Basic netowork client.
* forking_server.c
  * Network based forking server.
* select_server.c
  * Forking server code with example `select()` useage.
* select_client.c
  * Basic client code with example `select()` usage.
* makefile
  * make: Creates forking server/client programs.
  * make select: Creates select server/client programs.

## For a good overall review of sockets try [Beej's Guide to Network Programming](http://beej.us/guide/bgnet/html/single/bgnet.html)

## Select Statement
* The `select()` function allows a program to handle input from multiple filed descriptors (and other things).
* You can combine multiple file descriptors into a single set of inputs.
* `select()` will block until any of the set file descriptors are ready.
* Once triggerd, you can check which file descriptor(s) became ready, then act accordingly.
* **select_server.c** and **select_client.c** have example select code.
