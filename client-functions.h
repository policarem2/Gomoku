/* Description: functions for the client-main.c

*/
#ifndef CLIENT_FUNCTIONS
#define CLIENT_FUNCTIONS
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include "crypt.h"

typedef struct PLAYER
{
    char user[100];
    char opponent[100];
    int p_no;
    int nMoves;
    int gameOver;
    char stone;
    int x, y;
    char board[8][8];
} Player;

typedef struct LOGIN
{
    int   wins;
    int   losses;
    int   ties;
} Login;

// initaialize the player
void initializePlayer(Player *player);

//initialize stats
void initializeStats(Login *login);

// get the players move
void getMove(Player *player);

// Send the move to the server
void sendMove(int server_socket, Player *player);

// Send User to server
void sendUser(int server_socket, char *user, int length);

//receive scores
void scoreboard(Player *player, int server_socket);

// Set the player struct
void setPlayer(Player *player);

// Set the opponent player
void setOpponent(Player *player);

// set position of the board
void setPosition(Player *player);

// if won display win or loss to each player
void ifWon(Player *player, int server_socket);

// if tie display ties to players
void ifTie(Player *player, int server_socket);

// check if game over
void checkIfGameOver(Player *player, int server_socket);

// display the board
void displayBoard(Player *player);

// get the server connection
int get_server_connection(char *hostname, char *port);

// print IP info from getaddrinfo()
void print_ip(struct addrinfo *ai);

#endif