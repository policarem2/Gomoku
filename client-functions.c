/* Description: Client-functions.c
**
*/
#include "client-functions.h"

void initializePlayer(Player *player)
{
   player->p_no = 0;
   player->nMoves = 64;
   player->gameOver = 0;
   player->stone = 'B';
   player->x = -1;
   player->y = -1;
   for (int column = 0; column < 8; column++)
   {
      for (int row = 0; row < 8; row++)
      {
         player->board[column][row] = '-';
      }
   }
}

void sendUser(int server_socket, char *user, int length)
{
   send(server_socket, user, length, 0);
}

void initializeStats(Login *login){
   int   wins = 0;
   int   losses = 0;
   int   ties = 0;
}

void scoreboard(Player *player, int server_socket){
   Login *player1 = (Login*)malloc(sizeof(Login));
   Login *player2 = (Login*)malloc(sizeof(Login));
   initializeStats(player1);
   initializeStats(player2);
   recv(server_socket, &player1->wins, sizeof(int), 0);
   recv(server_socket, &player2->wins, sizeof(int), 0);
   recv(server_socket, &player1->losses, sizeof(int), 0);
   recv(server_socket, &player2->losses, sizeof(int), 0);
   recv(server_socket, &player1->ties, sizeof(int), 0);
   recv(server_socket, &player2->ties, sizeof(int), 0);

   printf("Player: %s Wins: %d Losses: %d Ties: %d\n", player->user, player1->wins, player1->losses, player1->ties);
   printf("Player: %s Wins: %d Losses: %d Ties: %d\n", player->opponent, player2->wins, player2->losses, player2->ties);
   free(player1);
   free(player2);
}

void getMove(Player *player)
{
   char *input = (char *)malloc(sizeof(char));
   printf("Enter row:\n");
   fgets(input, sizeof(input), stdin);
   sscanf(input, "%d", &player->x);
   printf("Enter column:\n");
   fgets(input, sizeof(input), stdin);
   sscanf(input, "%d", &player->y);
   free(input);
}

void sendMove(int server_socket, Player *player)
{
   send(server_socket, &player->x, sizeof(player->x), 0);
   send(server_socket, &player->y, sizeof(player->y), 0);
}

void setPosition(Player *player)
{
   player->board[player->x][player->y] = player->stone;
}

void ifWon(Player *player, int server_socket)
{
   if (strcmp(&player->stone, "W") == 0)
   {
      if (player->p_no == 1)
      {
         printf("⋆——————✧◦♚◦✧——————⋆\n");
         printf("   ✿∗˵YOU WIN˵∗✿\n");
         printf("⋆——————✧◦♚◦✧——————⋆\n");
         printf("%s lost!\n", player->opponent);
         scoreboard(player, server_socket);
      }
      else
      {
         printf("∗˵%s WINS˵∗\n", player->opponent);
         printf("╔═══*.·:·.☽✧    ✦    ✧☾.·:·.*═══╗\n");
         printf(" “ψ (｀∇´) ψ YOU LOSE “ψ (｀∇´) ψ\n");
         printf("╚═══*.·:·.☽✧    ✦    ✧☾.·:·.*═══╝\n");
         scoreboard(player, server_socket);
      }
   }
   else
   {
      if (player->p_no == 2)
      {
         printf("⋆——————✧◦♚◦✧——————⋆\n");
         printf("   ✿∗˵YOU WIN˵∗✿\n");
         printf("⋆——————✧◦♚◦✧——————⋆\n");
         printf("%s lost!\n", player->opponent);
         scoreboard(player, server_socket);
      }
      else
      {
         printf("∗˵%s WINS˵∗\n", player->opponent);
         printf("╔═══*.·:·.☽✧    ✦    ✧☾.·:·.*═══╗\n");
         printf(" “ψ (｀∇´) ψ YOU LOSE “ψ (｀∇´) ψ\n");
         printf("╚═══*.·:·.☽✧    ✦    ✧☾.·:·.*═══╝\n");
         scoreboard(player, server_socket);
      }
   }
   displayBoard(player);
}

void ifTie(Player *player, int server_socket)
{
   displayBoard(player);
   printf("(ง •̀_•́)ง TIE! Game over.\n");
   scoreboard(player, server_socket);
}

void checkIfGameOver(Player *player, int server_socket)
{
   if (player->gameOver == 1)
   {
      ifWon(player, server_socket);
   }
   if (player->nMoves == 0)
   {
      ifTie(player, server_socket);
   }
}

void displayBoard(Player *player)
{
   printf("  0 1 2 3 4 5 6 7");
   printf("\n -----------------\n");
   for (int column = 0; column < 8; column++)
   {
      printf("%d|", column);
      for (int row = 0; row < 8; row++)
      {
         printf("%c|", player->board[column][row]);
      }
      printf("\n");
   }
   printf(" -----------------\n");
}

void setPlayer(Player *player)
{
   if (player->p_no == 1)
   {
      player->stone = 'W';
   }
   else
   {
      player->stone = 'B';
   }
}

void setOpponent(Player *player)
{
   if (player->p_no == 1)
   {
      player->stone = 'B';
   }
   else
   {
      player->stone = 'W';
   }
}

int get_server_connection(char *hostname, char *port)
{
   int serverfd;
   struct addrinfo hints, *servinfo, *p;
   int status;

   memset(&hints, 0, sizeof hints);
   hints.ai_family = PF_UNSPEC;
   hints.ai_socktype = SOCK_STREAM;

   if ((status = getaddrinfo(hostname, port, &hints, &servinfo)) != 0)
   {
      printf("getaddrinfo: %s\n", gai_strerror(status));
      return -1;
   }

   print_ip(servinfo);
   for (p = servinfo; p != NULL; p = p->ai_next)
   {
      // create a socket
      if ((serverfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
      {
         printf("socket socket \n");
         continue;
      }

      // connect to the server
      if ((status = connect(serverfd, p->ai_addr, p->ai_addrlen)) == -1)
      {
         close(serverfd);
         printf("socket connect \n");
         continue;
      }
      break;
   }

   freeaddrinfo(servinfo);

   if (status != -1)
      return serverfd;
   else
      return -1;
}

void print_ip(struct addrinfo *ai)
{
   struct addrinfo *p;
   void *addr;
   char *ipver;
   char ipstr[INET6_ADDRSTRLEN];
   struct sockaddr_in *ipv4;
   struct sockaddr_in6 *ipv6;
   short port = 0;

   for (p = ai; p != NULL; p = p->ai_next)
   {
      if (p->ai_family == AF_INET)
      {
         ipv4 = (struct sockaddr_in *)p->ai_addr;
         addr = &(ipv4->sin_addr);
         port = ipv4->sin_port;
         ipver = "IPV4";
      }
      else
      {
         ipv6 = (struct sockaddr_in6 *)p->ai_addr;
         addr = &(ipv6->sin6_addr);
         port = ipv4->sin_port;
         ipver = "IPV6";
      }
      inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
      printf("serv ip info: %s - %s @%d\n", ipstr, ipver, ntohs(port));
   }
}

void GameOver(int status);
