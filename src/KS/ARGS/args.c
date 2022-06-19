// PHOTON INTERNEL "Wave" Kernel
// Code is governed by the GPL-2.0 license.
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "../../utils.h"
#include "../../klibc/string.h"
#include "../../serial/serial.h"
static char *tokens[100]; // up to 100 args cause I'm lazy
static int count = 1;

// kernel options
bool devbuild = false;
void parse(char *args)
{
    
    char *token = strtok(args, " ");
    tokens[0] = token;
    // loop through the string to extract all other tokens
    while( token != NULL ) 
    {
        token = strtok(NULL, " ");
        tokens[count] = token;
        count++;
    }
    count--;
}

void init(char* args)
{
    parse(args);
    for (int i = 0; i != count; i++)
    {
        char *a = strtok(tokens[i], "=");
        char *b = strtok(NULL, "=");
        // this is ass this makes me vomit
        // I'm sorry for this
        
        if (strcmp(a, "devbuild") == 0)
        {
            if (strcmp(b, "yes") == 0)
            {
                devbuild = true;
            }
            else
            {
                devbuild = false;
            }
        }
    }
}
