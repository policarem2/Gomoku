/* File Name: CommonFunctions.c
   Description: holds basic functions,
   inlcudes CommonFunctions.h
*/
#define MAXSIZE 100
#include "CommonFunctions.h"

void initializeGame(Game *game)
{
    game->nMoves = 64;
    game->gameOver = 0;
    game->stone = 'W';
    game->x = -1;
    game->y = -1;
    for (int column = 0; column < 8; column++)
    {
        for (int row = 0; row < 8; row++)
        {
            game->board[column][row] = '-';
        }
    }
}

void initializeStats(Login *login){
    login->wins = 0;
    login->losses = 0; 
    login->ties = 0;
}


void displayBoard(Game *game)
{
    printf("  0 1 2 3 4 5 6 7");
    printf("\n -----------------\n");
    for (int column = 0; column < 8; column++)
    {
        printf("%d|", column);
        for (int row = 0; row < 8; row++)
        {
            printf("%c|", game->board[column][row]);
        }
        printf("\n");
    }
    printf(" -----------------\n");
}

void setPosition(Game *game)
{
    game->board[game->x][game->y] = game->stone;
}

void switchPlayer(Game *game)
{
    if (strcmp(&game->stone, "W") == 0)
    {
        game->stone = 'B';
    }
    else
    {
        game->stone = 'W';
    }
}

int verifyInput(Game *game)
{
    if (game->x < 0 || game->x > 7 || game->y < 0 || game->y > 7)
    {
        printf("(%d, %d) is out of bounds.\n", game->x, game->y);
        return -1;
    }
    else if (game->board[game->x][game->y] != '-')
    {
        printf("(%d, %d) is taken.\n", game->x, game->y);
        return -1;
    }
    return 0;
}

void sendMoveData(Players *player, Game *game)
{
    // SocketA
    send(player->socketA, &game->gameOver, sizeof(game->gameOver), 0);
    send(player->socketA, &game->nMoves, sizeof(game->nMoves), 0);
    send(player->socketA, &game->x, sizeof(game->x), 0);
    send(player->socketA, &game->y, sizeof(game->y), 0);
    send(player->socketA, &game->stone, sizeof(game->stone), 0);
    // SocketB
    send(player->socketB, &game->gameOver, sizeof(game->gameOver), 0);
    send(player->socketB, &game->nMoves, sizeof(game->nMoves), 0);
    send(player->socketB, &game->x, sizeof(game->x), 0);
    send(player->socketB, &game->y, sizeof(game->y), 0);
    send(player->socketB, &game->stone, sizeof(game->stone), 0);
}

void sendScores(Players *players){
    send(players->socketA, &players->playerA->wins, sizeof(int), 0);
    send(players->socketB, &players->playerB->wins, sizeof(int), 0);
    send(players->socketA, &players->playerB->wins, sizeof(int), 0);
    send(players->socketB, &players->playerA->wins, sizeof(int), 0);
    

    send(players->socketA, &players->playerA->losses, sizeof(int), 0);
    send(players->socketB, &players->playerB->losses, sizeof(int), 0);
    send(players->socketA, &players->playerB->losses, sizeof(int), 0);
    send(players->socketB, &players->playerA->losses, sizeof(int), 0);

    send(players->socketA, &players->playerA->ties, sizeof(int), 0);
    send(players->socketB, &players->playerB->ties, sizeof(int), 0);
    send(players->socketA, &players->playerB->ties, sizeof(int), 0);
    send(players->socketB, &players->playerA->ties, sizeof(int), 0);
    }

void ifWon(Game *game, Players *players)
{
    printf("✿∗˵╰༼✪ᗜ✪༽╯˵∗✿\n");
    if (strcmp(&game->stone, "B") == 0)
    {
        printf("✿∗˵PLAYER 1 WINS˵∗✿\n");
        players->playerA->wins = players->playerA->wins+1;
        printf("%d", players->playerA->wins);
        players->playerB->losses = players->playerB->losses+1;
        displayBoard(game);
    }
    else
    {
        printf("✿∗˵PLAYER 2 WINS˵∗✿\n");
        players->playerB->wins = players->playerB->wins+1;
        players->playerA->losses = players->playerA->losses+1;
        displayBoard(game);
    }
    sendScores(players);
}

void ifTie(Game *game, Players *players)
{
    displayBoard(game);
    players->playerA->losses = players->playerA->losses+1;
    players->playerB->wins = players->playerB->wins+1;
    sendScores(players);
    printf("(ง •̀_•́)ง TIE! Game over.");
}

void switchSocket(Players *players, int *clientSocket)
{
    if (*clientSocket == players->socketA)
    {
        *clientSocket = players->socketB;
    }
    else
    {
        *clientSocket = players->socketA;
    }
}

int get_server_socket(char *hostname, char *port)
{
    struct addrinfo hints, *servinfo, *p;
    int status;
    int server_socket;
    int yes = 1;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if ((status = getaddrinfo(hostname, port, &hints, &servinfo)) != 0)
    {
        printf("getaddrinfo: %s\n", gai_strerror(status));
        exit(1);
    }

    for (p = servinfo; p != NULL; p = p->ai_next)
    {
        // step 1: create a socket
        if ((server_socket = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1)
        {
            printf("socket socket \n");
            continue;
        }
        // if the port is not released yet, reuse it.
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
        {
            printf("socket option\n");
            continue;
        }

        // step 2: bind socket to an IP addr and port
        if (bind(server_socket, p->ai_addr, p->ai_addrlen) == -1)
        {
            printf("socket bind \n");
            continue;
        }
        break;
    }
    print_ip(servinfo);
    freeaddrinfo(servinfo); // servinfo structure is no longer needed. free it.

    return server_socket;
}
// Analogy: get a phone and sign up for service
int start_server(char *hostname, char *port, int backlog)
{
    int status = 0;
    int serv_socket = get_server_socket(hostname, port);
    // Analogy: ask phone company to activate the phone.
    if ((status = listen(serv_socket, backlog)) == -1)
    {
        printf("socket listen error\n");
    }
    return serv_socket;
}

// Analogy: Answer a call on your phone
int accept_client(int serv_sock)
{
    int reply_sock_fd = -1;
    socklen_t sin_size = sizeof(struct sockaddr_storage);
    struct sockaddr_storage client_addr;
    char client_printable_addr[INET6_ADDRSTRLEN];

    // accept a connection request from a client
    // the returned file descriptor from accept will be used
    // to communicate with this client.
    if ((reply_sock_fd = accept(serv_sock, (struct sockaddr *)&client_addr, &sin_size)) == -1)
    {
        printf("socket accept error\n");
    }
    else
    {
        // here is for info only, not really needed.
        inet_ntop(client_addr.ss_family, get_in_addr((struct sockaddr *)&client_addr), client_printable_addr, sizeof client_printable_addr);
        printf("server: connection from %s at port %d\n", client_printable_addr, ((struct sockaddr_in *)&client_addr)->sin_port);
    }
    return reply_sock_fd;
}

// ======= HELP FUNCTIONS =========== //
/* the following is a function designed for testing.
// it prints the ip address and port returned from
// getaddrinfo() function */
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

void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
    {
        printf("ipv4\n");
        return &(((struct sockaddr_in *)sa)->sin_addr);
    }
    else
    {
        printf("ipv6\n");
        return &(((struct sockaddr_in6 *)sa)->sin6_addr);
    }
}
