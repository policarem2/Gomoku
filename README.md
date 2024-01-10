# Description
   This is a client server application of the game Gomoku which is similar to connect 4. This application supports multiple games being run at once. 


# TO compile both the server and the clients:
   1. compile the server program:
        gcc -lpthread -o Gomoku GameController.c GameMaster.c database.c crypt.c CommonFunctions.c -lcrypt -I/usr/include/postgresql -lpq
   2. compile the cliient program:
        gcc -o client client-main.c client-functions.c crypt.c -lcrypt

# TO Run The Server and Client:

   1. run the server program the server that is specified in your code: 
        ./Gomoku PORT#
      -- PORT# choose any free port to use

   2. run the first client on a different server: 
        ./client PORT#
      -- PORT# choose the same port as the server

   3. run the second client on a different server: 
        ./client PORT#
      -- PORT# hoose the same port as the server

   4. To terminate the server and clients 
      Press CTRL + C
