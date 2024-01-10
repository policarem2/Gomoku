// compile:  gcc -o client client-main.c client-functions.c crypt.c -lcrypt
// run: ./client
// port #:
#include "client-functions.h"

#define MAX_SIZE 100
#define HOST ""

void player_no(Player *player)
{
   if (player->p_no == 1)
   {
      printf("\n");
      printf("Player 1: %s\nOpponent: %s\n", &player->user, &player->opponent);
   }
   else
   {
      printf("\n");
      printf("Player 2: %s\nOpponent: %s\n", &player->user, &player->opponent);
   }
}

void beginGame(Player *player, int server_socket)
{
   printf("       ⋆｡ﾟ☁︎｡⋆｡ ﾟ☾ ﾟ｡ ୨ʕ˶ᵔᴥᵔ˶ʔ୧ ⋆｡ﾟ☁︎｡⋆｡ ﾟ☾ ﾟ｡\n");
   printf("(〜^∇^)〜 Welcome to Gomoku! Let's begin!〜(^∇^〜)\n");
   printf("         ┈┈┈◦•✩•◦┈┈◦•✩•◦┈┈◦•✩•◦┈┈┈\n");
   printf("          ごもくへようこそ！始めよう！\n");
   printf("         ┈┈┈◦•✩•◦┈┈◦•✩•◦┈┈◦•✩•◦┈┈┈\n");
   printf("");
   player_no(player);
   while (player->gameOver == 0 && player->nMoves != 0)
   {
      int validStatus = -1;
      if ((player->p_no == 1 && player->stone == 'B') || (player->p_no == 2 && player->stone == 'W'))
      {
         while (validStatus == -1)
         {
            getMove(player);
            sendMove(server_socket, player);
            recv(server_socket, &validStatus, sizeof(validStatus), 0);
            if (validStatus == -1)
               printf("Invalid input\n");
         }
      }
      else
      {
         printf("Waiting for oponents move...\n");
      }
      setPosition(player);
      recv(server_socket, &player->gameOver, sizeof(player->gameOver), 0);
      recv(server_socket, &player->nMoves, sizeof(player->nMoves), 0);
      recv(server_socket, &player->x, sizeof(player->x), 0);
      recv(server_socket, &player->y, sizeof(player->y), 0);
      recv(server_socket, &player->stone, sizeof(player->stone), 0);
      setPosition(player);
      displayBoard(player);
   }
   checkIfGameOver(player, server_socket);
   free(player);
   close(server_socket);
}

void FreeVariables(char *username, char *email, char *password)
{
   free(username);
   free(email);
   free(password);
}
void authenticateUser(void *arg, int server_socket)
{
   Player *player = (Player *)arg;
   char stat[10];
   int validStatus = -1;
   int status = 0;
   while (validStatus == -1)
   {
      char *username = (char *)malloc(MAX_SIZE);
      username[0] = '\0';
      char *email = (char *)malloc(MAX_SIZE);
      char *password = (char *)malloc(MAX_SIZE);
      printf("New Users (Press 1)\n");
      printf("Returning Users (Press 2)\n");
      fgets(stat, sizeof(stat), stdin);
      sscanf(stat, "%d", &status);
      if (status == 1 || status == 2)
      {
         if (status == 1)
         {
            printf("Please enter username: \n");
            fgets(username, MAX_SIZE, stdin);
            strcpy(player->user, username);
            printf("Please enter email: \n");
            fgets(email, MAX_SIZE, stdin);
            email[strcspn(email, "\n")] = '\0';
            printf("Please enter password: \n");
            strcpy(password, getPlainPass());
            password[strcspn(password, "\n")] = '\0';
         }
         else if (status == 2)
         {
            printf("Please enter email: \n");
            fgets(email, MAX_SIZE, stdin);
            email[strcspn(email, "\n")] = '\0';
            printf("Please enter password: \n");
            strcpy(password, getPlainPass());
            password[strcspn(password, "\n")] = '\0';
         }
         send(server_socket, username, MAX_SIZE, 0);
         send(server_socket, email, MAX_SIZE, 0);
         send(server_socket, password, MAX_SIZE, 0);
         FreeVariables(username, email, password);
         recv(server_socket, &validStatus, sizeof(validStatus), 0);
      }
   }
   printf("Please wait for game to begin!\n");
   recv(server_socket, player->user, sizeof(player->user), 0);
   recv(server_socket, player->opponent, sizeof(player->opponent), 0);
}

int main(int argc, char *argv[])
{
   int start = 0;
   int status = -1;
   int server_socket = get_server_connection(HOST, argv[1]);
   Player *player = (Player *)malloc(sizeof(Player));
   initializePlayer(player);
   recv(server_socket, &player->p_no, sizeof(player->p_no), 0);
   authenticateUser(player, server_socket);
   beginGame(player, server_socket);
}