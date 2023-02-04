#include "utility.h"

pid_t childPid;
vectorstring cmds;
int scaninterrupt = 0;

void run();
void sig_handler(int signum)
{
    scaninterrupt = 1;
    printf("\n");
    if(childPid > 0) 
    {
        kill(childPid, SIGINT);
        childPid = -1;
    }
    fflush(stdout);
}
void run()
{
    childPid = -1;
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
    if(stringEmpty(s)) run();
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
    wait(NULL);
}

int main()
{
    signal(SIGINT, sig_handler);
    siginterrupt(SIGINT, 1);
    cmds.capacity = 500;
    cmds.size = 0;
    cmds.data = (char**)malloc(sizeof(char*) * 500);
    while(1)
    {
        run();
    }
    return 0;
}