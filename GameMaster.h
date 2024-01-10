/* File name:  GameMaster.h
   Description:
   #include "GameMaster.h" in your program.
*/
#ifndef GAMEMASTER_H
#define GAMEMASTER_H

#include "CommonFunctions.h"

// makes threads to check for the win vertically and horizontally
void CheckForWin(Game *game);

// accepts player input
void playerInput(Players *players, void *arg);

#endif
