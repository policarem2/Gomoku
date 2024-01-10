/*  File name:   GameMaster.c
    Description: Contains all of the threads within the server
*/
// #define _POSIX_SOURCE
#include "GameMaster.h"

void *checkForHorizontalWin(void *ptr)
{
    Game *game = (Game *)ptr;
    int currentRow = 0;
    int currentStreak = 0;
    char streakOwner;
    for (int row = 0; row < 8; row++)
    {
        for (int column = 0; column < 8; column++)
        {
            if (currentStreak == 5)
            {
                game->gameOver = 1;
                pthread_exit(NULL);
            }
            if (currentRow != row)
            {
                currentRow++;
                currentStreak = 0;
            }
            if (game->board[column][row] == game->stone)
            {
                currentStreak++;
            }
            else
            {
                currentStreak = 0;
            }
        }
    }
}

void *checkForVerticalWin(void *ptr)
{
    Game *game = (Game *)ptr;
    int currentCollumn = 0;
    int currentStreak = 0;
    char streakOwner;
    for (int column = 0; column < 8; column++)
    {
        for (int row = 0; row < 8; row++)
        {
            if (currentStreak == 5)
            {
                game->gameOver = 1;
                pthread_exit(NULL);
            }
            if (currentCollumn != column)
            {
                currentCollumn++;
                currentStreak = 0;
            }
            if (game->board[column][row] == game->stone)
            {
                currentStreak++;
            }
            else
            {
                currentStreak = 0;
            }
        }
    }
}

void CheckForWin(Game *game)
{
    pthread_t horizontal_thread, vertical_thread;

    pthread_create(&horizontal_thread, NULL, &checkForHorizontalWin, (void *)game);
    pthread_create(&vertical_thread, NULL, &checkForVerticalWin, (void *)game);

    pthread_join(horizontal_thread, NULL);
    pthread_join(vertical_thread, NULL);
}

void playerInput(Players *player, void *arg)
{
    Game *game = (Game *)arg;
    int clientSocket = player->socketA;
    while (game->gameOver == 0 && game->nMoves != 0)
    {
        int validStatus = -1;
        while (validStatus == -1)
        {
            printf("Select a position to place your stone! \n");
            printf("Enter row:\n");
            recv(clientSocket, &game->x, sizeof(game->x), 0);
            printf("Enter column:\n ");
            recv(clientSocket, &game->y, sizeof(game->y), 0);
            validStatus = verifyInput(game);
            send(clientSocket, &validStatus, sizeof(validStatus), 0);
        }
        setPosition(game);
        game->nMoves--;
        CheckForWin(game);
        sendMoveData(player, game);
        displayBoard(game);
        switchPlayer(game);
        switchSocket(player, &clientSocket);
    }
    if (game->gameOver == 1)
    {
        ifWon(game, player);
    }
    if (game->nMoves == 0)
    {
        ifTie(game, player);
    }
    close(player->socketA);
    close(player->socketB);
    free(game);
}
