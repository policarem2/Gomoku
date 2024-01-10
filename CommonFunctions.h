/* File name:   CommonFunctions.h
  Description: the header file,
  and holds the game Struct
*/

#ifndef COMMONFUNCTIONS_H
#define COMMONFUNCTIONS_H

#include <pthread.h>
#include <time.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include "crypt.h"
#include "database.h"
#include "LinkedList.h"

typedef struct GAME
{
  int nMoves;
  int gameOver;
  char stone;
  int x, y;
  char board[8][8];
} Game;



// initialize the players
void initializePlayers(Players *players, int player1_fd, int player2_fd, char *user1, char *user2);

// initialize the game
void initializeGame(Game *game);

//initialize login
void initializeStats(Login *login);

//start a new thread for two players
void *subserver(void * reply_sock_fd_as_ptr);

// Function to show board
void displayBoard(Game *game);

// Store the x,y value of positon
void setPosition(Game *game);

// Switches the player
void switchPlayer(Game *game);

// check if the input is valid
int verifyInput(Game *game);

// send move data to update
void sendMoveData(Players *players, Game *game);

//sends scoreboard
void sendScores(Players *players);

// If a won is found
void ifWon(Game *game, Players *players);

// If the board fills up
void ifTie(Game *game, Players *players);

// switch the player socket
void switchSocket(Players *players, int *clientSocket);

// start the server
int start_server(char *hostname, char *port, int backlog);

// accept a connection from client
int accept_client(int serv_sock);

// get internet address
void *get_in_addr(struct sockaddr *sa);

// get a server socket
int get_server_socket(char *hostname, char *port);

// print IP info from getaddrinfo()
void print_ip(struct addrinfo *ai);


#endif