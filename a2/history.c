#include "history.h"

const char *history_file = ".cmd_history";
pid_t childPid;
vectorstring cmds;
FILE* fphist;

int handleup(int count, int key)
{
    printf("up");
    return 0;
}

int handledown(int count, int key)
{
    printf("down");
    return 0;
}

void initialize_readline(){

    // map handleup function to up arrow key
    rl_bind_keyseq("\e[A", handleup);

    // map handledown function to down arrow key
    rl_bind_keyseq("\e[B", handledown);

}

void run()
{

    childPid = -1;
    char *s, *inputfile, *outputfile;
    
    s = (char *)malloc(1000 * sizeof(char));
    inputfile = (char *)malloc(1000 * sizeof(char));
    outputfile = (char *)malloc(1000 * sizeof(char));

    s = readline(PROMPT);
    // getchar();

    printf("Line : %s\n", rl_line_buffer);

    // add entered commands to history file
    fphist = fopen(history_file, "a");
    fputs(s, fphist);
    putc('\n', fphist);
    fclose(fphist);

    if(!strcmp(s, "exit")) 
        exit(0);

    push_back(&cmds, s);
    vectorstring v = split(s);
    
    FILE* fpin;
    if (!strcmp(v.data[0], "cd"))
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
    initialize_readline();
    cmds.capacity = 500;
    cmds.size = 0;
    cmds.data = (char**)malloc(sizeof(char*) * 500);

    while (1){

        run();
    }
    return 0;
}