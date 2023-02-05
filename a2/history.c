#include "history.h"

const char *history_file = ".cmd_history";
pid_t childPid;
vectorstring cmds;

int handleup(int count, int key)
{
    printf("Hello up!\n");
}

int handledown(int count, int key)
{
    printf("Hello down!\n");
}

void run()
{
    childPid = -1;
    char s[1000], inputfile[1000], outputfile[1000];
    printf("%s", getcwd((char*)NULL, (size_t)0));
    printf(PROMPT);
    scanf("%[^\n]s", s);
    getchar();

    // add entered commands to history file
    FILE* fphist = fopen(history_file, "a");
    fputs(s, fphist);
    putc('\n', fphist);
    fclose(fphist);

    if(!strcmp(s, "exit")) 
        exit(0);

    push_back(&cmds, s);
    vectorstring v = split(s);
    
    FILE* fpin;
    if(!strcmp(v.data[0], "cd"))
    {
        chdir(v.data[1]);
        run();
    }
    for (int i=0; i<v.size; i++)
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
        execvp(args[0], args);
    }
    wait(NULL);
}

int main()
{
    // rl_add_defun("handleup", handleup, 38);
    // rl_add_defun("handledown", handledown, 40);

    cmds.capacity = 500;
    cmds.size = 0;
    cmds.data = (char**)malloc(sizeof(char*) * 500);
    while(1)
    {
        run();
    }
    return 0;
}