# FinalTermProject-FTP
## Daniel Regassa, Eric Zhang, and Nikita Borisov (pd05)

## Description:
   File server similar to GitHub that works as a file transfer protocol. The server-side file database consists of a single
  directory with all the user files in it. Users have accounts with usernames and passwords. Whenever a client pushes a file
  not yet in the database, one will be created and the user gains pull and push permissions to that file. They can share
  pull, push, or both permissions with other users(who can then share those perimissions with others). Only one client can pull
  at a time and other users have to wait until they repush into the database.
  
## Instructions on how to use your project:
   Enter ```$make``` and ```$make run```. Start up a server by answering 's' to the first question. Then ```$make run``` on a 
  different terminal(potentially on a different computer) and answer 'c' to first question followed by entering the IP address
  of the server you have started up and with to connect to. 
   After the connection is established, follow instructions to create an account and play around with pushing,pulling,sharing,
   and viewing. You can use the files ```example.txt``` and ```example2.txt``` for testing(first push them from the
   ```FinalTermProject-FTP``` directory).
