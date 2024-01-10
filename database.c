#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libpq-fe.h>
#include "database.h"

PGconn *GetConnection()
{
    PGconn *conn = PQconnectdb("user= password= host= dbname=");

    if (PQstatus(conn) == CONNECTION_BAD)
    {

        fprintf(stderr, "Connection to database failed: %s\n",
                PQerrorMessage(conn));
        PQfinish(conn);
    }
    return conn;
}

int RegisterPlayer(Login *player)
{

    PGconn *conn = GetConnection();

    char sql[500];
    snprintf(sql, sizeof(sql), "INSERT INTO player VALUES('%s', '%s', '%s',0,0,0)", player->email, player->password, player->name);

    PGresult *res = PQexec(conn, sql);

    if (PQresultStatus(res) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Registering user failed: %s\n",
                PQerrorMessage(conn));
        PQclear(res);
        PQfinish(conn);
        return 0;
    }
    PQclear(res);
    PQfinish(conn);
    return 1;
}

char *GetPasswordByEmail(char *email)
{
    PGconn *conn = GetConnection();
    char sql[1024];
    snprintf(sql, sizeof(sql), "SELECT password FROM player where email = '%s'", email);
    PGresult *res = PQexec(conn, sql);
    char *password = NULL;
    if (PQresultStatus(res) != PGRES_TUPLES_OK || PQntuples(res) == 0)
    {
        printf("No data retrieved\n");
    }
    else
    {
        password = PQgetvalue(res, 0, 0);
    }

    PQclear(res);
    PQfinish(conn);
    return password;
}

int GetPlayerByEmail(Login *player)
{
    PGconn *conn = GetConnection();

    char sql[500];
    snprintf(sql, sizeof(sql), "SELECT * FROM player where email = '%s'", player->email);
    PGresult *res = PQexec(conn, sql);

    if (PQresultStatus(res) != PGRES_TUPLES_OK)
    {
        printf("No data retrieved\n");
        return 1;
    }
    player->name = PQgetvalue(res, 0, 2);
    player->wins = atoi(PQgetvalue(res, 0, 3));
    player->losses = atoi(PQgetvalue(res, 0, 4));
    player->ties = atoi(PQgetvalue(res, 0, 5));

    PQclear(res);
    PQfinish(conn);
    return 0;
}

int UpdatePlayerScoreboard(Players *players)
{

    PGconn *conn = GetConnection();
    char sql1[500];
    char sql2[500];
    snprintf(sql1, sizeof(sql1),
             "UPDATE player SET wins =%d, losses=%d, ties=%d WHERE email ='%s'",
             players->playerA->wins,
             players->playerA->losses,
             players->playerA->ties,
             players->playerA->email);

    snprintf(sql2, sizeof(sql2),
             "UPDATE player SET wins =%d, losses=%d, ties=%d WHERE email ='%s'",
             players->playerB->wins,
             players->playerB->losses,
             players->playerB->ties,
             players->playerB->email);

    PGresult *res1 = PQexec(conn, sql1);
    PGresult *res2 = PQexec(conn, sql2);

    if (PQresultStatus(res1) != PGRES_COMMAND_OK || PQresultStatus(res2) != PGRES_COMMAND_OK)
    {
        fprintf(stderr, "Updating scoreboards failed: %s\n",
                PQerrorMessage(conn));
        PQclear(res1);
        PQclear(res2);
        PQfinish(conn);
        return 1;
    }
    PQclear(res1);
    PQclear(res2);
    PQfinish(conn);
    return 0;
}
