#ifndef __MAIN_H__
#define __MAIN_H__

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

pid_t childPid;
vectorstring cmds;

void push_back(vectorstring *v, char *s);
vectorstring split(char *s);
void handlectrlc(int sig);
pid_t childPid;
vectorstring cmds;

#endif
