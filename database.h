#ifndef DATABASE_H
#define DATABASE_H
#include <semaphore.h>
typedef struct LOGIN
{
    char *email;
    char *password;
    char *name;
    int wins;
    int losses;
    int ties;
} Login;

typedef struct PLAYERS
{
    int socketA;
    int socketB;
    Login *playerA;
    Login *playerB;
} Players;

int RegisterPlayer(Login *login);

char *GetPasswordByEmail(char *email);

int UpdatePlayerScoreboard(Players *players);
int GetPlayerByEmail(Login *player);

#endif
