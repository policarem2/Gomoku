
/* File name:   GameController.c
   Description: c program designed to run a two player game.
   Compile:  gcc -lpthread -o Gomoku GameController.c GameMaster.c database.c crypt.c CommonFunctions.c -lcrypt -I/usr/include/postgresql -lpq
   Run: ./Gomoku 17100
*/

#include "GameMaster.h"
#define HOST ""
#define BACKLOG 10
#define BUFFER_SIZE 256
#define HTTP_REQUEST_TYPE 1
#define GREETING_TYPE 2
#define PORT 17100
#define MAX_SIZE 100

sem_t sem1;
sem_t sem2;
sem_t sem3;

int main(int argc, char *argv[])
{
   sem_init(&sem1, 0, 1);
   sem_init(&sem2, 0, 1);
   sem_init(&sem3, 0, 1);
   int http_sock_fd; // http server socket
   int reply_sock_fd[2];

   http_sock_fd = start_server(HOST, argv[1], BACKLOG);
   if (http_sock_fd == -1)
   {
      printf("start server error\n");
      exit(1);
   }
   while (1)
   {
      pthread_t sub_serv;
      int i = 0;
      while (i != 2)
      {
         if ((reply_sock_fd[i] = accept_client(http_sock_fd)) == -1)
         {
            continue;
         }
         i++;
      }
      pthread_create(&sub_serv, NULL, &subserver, (void *)reply_sock_fd);
   }
   sem_destroy(&sem1);
   sem_destroy(&sem2);
   sem_destroy(&sem3);
   close(http_sock_fd);
   return 0;
}

int RegisterUser(Login *player)
{
   player->password = encode(player->password);
   int playerRegistered = RegisterPlayer(player);
   if (playerRegistered)
   {
      return 0;
   }
   else
   {
      return -1;
   }
}

void *authenticateUserA(void *arg)
{
   Players *players = (Players *)arg;
   Login *playerA = (Login *)malloc(sizeof(Login));
   initializeStats(playerA);
   playerA->name = malloc(MAX_SIZE);
   playerA->email = malloc(MAX_SIZE);
   playerA->password = malloc(MAX_SIZE);
   int validStatus = -1;
   while (validStatus == -1)
   {
      recv(players->socketA, playerA->name, MAX_SIZE, 0);
      recv(players->socketA, playerA->email, MAX_SIZE, 0);
      recv(players->socketA, playerA->password, MAX_SIZE, 0);
      if (playerA->name[0] != '\0')
      {
         validStatus = RegisterUser(playerA);
      }
      else
      {
         char *pw = GetPasswordByEmail(playerA->email);
         if (pw == NULL)
         {
            send(players->socketA, &validStatus, sizeof(validStatus), 0);
            continue;
         }
         if (authenticate(playerA->password, pw) == 1)
         {
            validStatus = 0;
            sem_wait(&sem1);
            int getPlayerStatus = GetPlayerByEmail(playerA);
            sem_post(&sem1);
         }
         else
         {
            send(players->socketA, &validStatus, sizeof(validStatus), 0);
            continue;
         }
      }
      send(players->socketA, &validStatus, sizeof(validStatus), 0);
   }
   players->playerA = playerA;
   pthread_exit(NULL);
}

void *authenticateUserB(void *arg)
{
   Players *players = (Players *)arg;
   Login *playerB = (Login *)malloc(sizeof(Login));
   initializeStats(playerB);
   playerB->name = malloc(MAX_SIZE);
   playerB->email = malloc(MAX_SIZE);
   playerB->password = malloc(MAX_SIZE);
   int validStatus = -1;
   while (validStatus == -1)
   {
      recv(players->socketB, playerB->name, MAX_SIZE, 0);
      recv(players->socketB, playerB->email, MAX_SIZE, 0);
      recv(players->socketB, playerB->password, MAX_SIZE, 0);
      if (playerB->name[0] != '\0')
      {
         validStatus = RegisterUser(playerB);
      }
      else
      {
         char *pw = GetPasswordByEmail(playerB->email);
         if (pw == NULL)
         {
            send(players->socketB, &validStatus, sizeof(validStatus), 0);
            continue;
         }
         if (authenticate(playerB->password, pw) == 1)
         {
            validStatus = 0;
            sem_wait(&sem2);
            int getPlayerStatus = GetPlayerByEmail(playerB);
            sem_post(&sem2);
         }
         else
         {
            send(players->socketB, &validStatus, sizeof(validStatus), 0);
            continue;
         }
      }
      send(players->socketB, &validStatus, sizeof(validStatus), 0);
   }
   players->playerB = playerB;
   pthread_exit(NULL);
}

void sendUserInfo(Players *players)
{
   send(players->socketA, players->playerA->name, MAX_SIZE, 0);
   send(players->socketB, players->playerB->name, MAX_SIZE, 0);
   send(players->socketA, players->playerB->name, MAX_SIZE, 0);
   send(players->socketB, players->playerA->name, MAX_SIZE, 0);
}

void *subserver(void *reply_sock_fd_as_ptr)
{
   int *reply_sock_fd = (int *)reply_sock_fd_as_ptr;
   int p1 = 1;
   int p2 = 2;
   pthread_t user1, user2;
   Players *players = (Players *)malloc(sizeof(Players));
   Game *game = (Game *)malloc(sizeof(Game));
   players->socketA = reply_sock_fd[0];
   players->socketB = reply_sock_fd[1];
   initializeGame(game);
   send(reply_sock_fd[0], &p1, sizeof(p1), 0);
   send(reply_sock_fd[1], &p2, sizeof(p2), 0);
   pthread_create(&user1, NULL, authenticateUserA, (void *)players);
   pthread_create(&user2, NULL, authenticateUserB, (void *)players);
   pthread_join(user1, NULL);
   pthread_join(user2, NULL);
   sendUserInfo(players);
   playerInput(players, game);
   sem_wait(&sem1);
   sem_wait(&sem2);
   sem_wait(&sem3);
   UpdatePlayerScoreboard(players);
   sem_post(&sem1);
   sem_post(&sem2);
   sem_post(&sem3);
   free(game);
   free(players);
   pthread_exit(NULL);
}
