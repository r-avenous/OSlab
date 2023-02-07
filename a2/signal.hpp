#ifndef __SIGNAL_H__
#define __SIGNAL_H__

#include "utility.hpp"
#include <sys/wait.h>

extern int scaninterrupt, background;
extern pid_t childPid;

void sigint_handler(int signum);
void sigtstp_handler(int signum);
void sigchld_handler(int signum);

#endif