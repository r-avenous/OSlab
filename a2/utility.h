#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#define PROMPT " [#] "

typedef struct __vectorstring 
{
    char **data;
    int size;
    int capacity;
}vectorstring;

void push_back(vectorstring *v, char *s);
vectorstring split(char *s);
void handlectrlc(int sig);
int stringEmpty(char *s);

#endif
