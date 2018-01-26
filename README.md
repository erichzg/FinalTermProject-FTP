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
   
   If there are server setup errors for networking, the initial port (9001) may be in use. Try changing the value of ```PORT```
   in ```networking.h``` to something else.
   
   ## Features:
   **Accounts file** stores usernames and passwords in the format ```<username1>:<password1>;<username2>:<password2>;...``` and 
   double encryption(with built in **crypt** function) is used(once on client side and once on server side) to prevent the
   stealing *meaningful* passwords as the go from client to server(still not fully secure though, this isn't SFTP). 
     
   Makes **semaphore** for each file using a **string to int hash function** for the semaphore keys. They get downed upon pulling
   and reset to 1 upon pushing.
     
   Stores pull and push permissions in separate **permissions files** in the format ```<file1>;<usernameA>:<usernameB>:|<file2><usernameB>:|```
   These files are used when granting push/pull requests, viewing push-able/pull-able files, and sharing permissions with
     other users.
   
   ## Attempted:
   File parsing into packets when sending file data between server/client.
   
   Preventing clients from pushing to file they can't pull due to it having been pulled(but not pushed back) by someone else.
   
   A multiple directory FTP.
   
   ## Bugs & Unresolved Issues:
   **Empty files** should not be pushed/pulled(when pushing, you must specify a path to a file that exists and contains
   some data) If client program freezes up due to this bug, it should be rerun. 
     
   If a client can't pull a file that is in use, they can push into that file(upping the semaphore) 
   and pull what they pushed. While they still won't be able to pull the original contents(since they overwrote on
   the push), this is still an issue since clients shouldn't be able to **gain pull accesss to files currently in use**
   (at least not by our rules).
     
   When sharing, a client can append any username without non-allowed chars(':',';','|'). This means they can **append
   usernames that don't exist** and append the same username **multiple times**(including readding themselves).
     
   Files larger than **32KB** cannot be pushed/pulled corrects and account/permission files are capped at **2KB** of information.
     
   Accounts, permissions, and database files **aren't secure** when testing on same computer(could theoretically be accessed
   by clients without going through server). Also passwords can be seen when typed into terminal. This is a high school cs
   project, so please don't store your actual passwords here.
     
   File ```base``` in fileDir should not be interracted with(will cause semaphore blockages). If client/server program freeze up due 
   to this bug, they should be rerun.
