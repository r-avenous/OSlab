#include "utility.h"

pid_t childPid;
vectorstring cmds;
int scaninterrupt = 0, background = 0;

void sigint_handler(int signum)
{
    scaninterrupt = 1;
    write(1, "\n", 1);
    if(childPid > 0) 
    {
        kill(childPid, SIGINT);
        childPid = -1;
    }
    fflush(stdout);
    fflush(stdin);
}
void sigtstp_handler(int signum)
{
    scaninterrupt = 1;
    write(1, "\n", 1);
    fflush(stdout);
    fflush(stdin);
}
void sigchld_handler(int signum)
{
    if(!background) return;
    scaninterrupt = 1;
    fflush(stdout);
    fflush(stdin);
}
void run()
{
    char s[1000], inputfile[1000], outputfile[1000];
    printf("%s", getcwd((char*)NULL, (size_t)0));
    printf(PROMPT);
    scanf("%[^\n]s", s);
    if(scaninterrupt) 
    {
        scaninterrupt = 0;
        return;
    }
    getchar();
    if(stringEmpty(s)) return;
    if(!strcmp(s, "exit")) exit(0);
    push_back(&cmds, s);
    vectorstring v = split(s);
    
    FILE* fpin;
    if(!strcmp(v.data[0], "cd"))
    {
        chdir(v.data[1]);
        return;
    }
    for(int i=0; i<v.size; i++)
    {
        if(!strcmp(v.data[i],"<"))
        {
            strcpy(inputfile, v.data[i+1]);
            fpin = fopen(inputfile, "r");
            i++;
        }
        else if(!strcmp(v.data[i], ">"))
        {
            strcpy(outputfile, v.data[i+1]);
            i++;
        }
        else if(!strcmp(v.data[i], "&"))
        {
            background = 1;
        }
    }
    childPid = fork();
    if(childPid == 0)
    {
        char* args[v.size + 1];
        for(int i = 0; i < v.size; i++)
        {
            args[i] = (char*)malloc(1+strlen(v.data[i]));
            strcpy(args[i], v.data[i]);
        }
        args[v.size] = NULL;
        if(execvp(args[0], args) == -1)
        {
            printf("Command not found\n");
            exit(0);
        }
    }
    if(!background) waitpid(childPid, NULL, 0);
    else waitpid(childPid, NULL, WNOHANG);
}

int main()
{
    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    signal(SIGCHLD, sigchld_handler);
    siginterrupt(SIGINT, 1);
    siginterrupt(SIGTSTP, 1);
    // siginterrupt(SIGCHLD, 1);
    
    cmds.capacity = 500;
    cmds.size = 0;
    cmds.data = (char**)malloc(sizeof(char*) * 500);
    while(1)
    {
        fflush(stdout);
        fflush(stdin);
        childPid = -1;
        background = 0;
        run();
    }
    return 0;
}