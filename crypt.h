
/*
** use generatesalt before all
** to encrypt use char* encode ( char password) - returns encrypted password
** to check password, use int authenticate ( password , encoded password)
**
*/

#ifndef CRYPT_H
#define CRYPT_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>


#define MAX_PSWD_SIZE 32

int getpasswd(char *pswd, int len);

void generatesalt(char salt[]);

char* encode(char *plainpswd);

int authenticate(char *loginpswd, char *savedpswd);

char *getPlainPass();

#endif