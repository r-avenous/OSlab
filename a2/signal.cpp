#include "signal.hpp"

int scaninterrupt = 0; 
int background = 0;
pid_t childPid = -1;

void sigint_handler(int signum)
{
    scaninterrupt = 1;
    cout << endl;
    if(childPid > 0) 
    {
        kill(childPid, SIGKILL);
        childPid = -1;
    }
}

void sigtstp_handler(int signum)
{
    scaninterrupt = 1;
    cout << endl;
}

void sigchld_handler(int signum)
{
    if(!background) return;
    scaninterrupt = 1;
    cout.flush();
}
