// compile:  gcc -lcrypt  crypt.c
/*
** use generatesalt before all
** to encrypt use char* encode ( char password) - returns encrypted password
** to check password, use int authenticate ( password , encoded password)
**
*/

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <termios.h>
#include "crypt.h"

#define MAX_PSWD_SIZE 32

int getpasswd(char *pswd, int len)
{
   struct termios current_settings;
   struct termios no_echo_settings;
   memset(pswd, 0, len);

   // get the current terminal settings
   if (tcgetattr(0, &current_settings))
   {
      printf("Error: accessing keyboard\n");
      return -1;
   }

   // save the current terminal settings
   memcpy(&no_echo_settings, &current_settings, sizeof(struct termios));

   // disable echoing
   no_echo_settings.c_lflag &= ~(ICANON | ECHO);
   if (tcsetattr(0, TCSANOW, &no_echo_settings))
   {
      printf("Error: accessing keyboard\n");
      return -1;
   }

   // read password with no echo
   int index = 0;
   char c;
   while (((c = fgetc(stdin)) != '\n') && (index < len - 1))
   {
      pswd[index++] = c;
   }
   pswd[index] = 0; /* null-terminate   */

   // restore terminal settings
   if (tcsetattr(0, TCSANOW, &current_settings))
   {
      printf("Error: accessing keyboard\n");
      return -1;
   }

   return index;
}

void generatesalt(char salt[])
{
   unsigned long seed[2];

   const char *const seedchars =
       "./0123456789ABCDEFGHIJKLMNOPQRST"
       "UVWXYZabcdefghijklmnopqrstuvwxyz";

   seed[0] = time(NULL);
   seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);

   for (int i = 0; i < 8; i++)
      salt[3 + i] = seedchars[(seed[i / 5] >> (i % 5) * 6) & 0x3f];

   return;
}

char *encode(char *plainpswd)
{
   char *savedpswd;
   char salt[] = "$1$........";

   // set set encryption algorithm to SHA-256
   crypt_set_format("sha256");

   // encode the plainpassword
   generatesalt(salt);
   char *encoded = crypt(plainpswd, salt);

   // retrieve the password from the crypt buffer
   savedpswd = (char *)malloc(256);
   strcpy(savedpswd, encoded);
   return savedpswd;
}

int authenticate(char *loginpswd, char *savedpswd)
{
   crypt_set_format("sha256");
   char *encodedloginpswd = crypt(loginpswd, savedpswd);
   return strcmp(encodedloginpswd, savedpswd) == 0;
}

char *getPlainPass()
{
   char *plainpswd = (char *)malloc(MAX_PSWD_SIZE);
   getpasswd(plainpswd, MAX_PSWD_SIZE);
   return plainpswd;
}
