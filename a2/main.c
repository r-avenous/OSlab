#include "main.h"

void push_back(vectorstring *v, char *s)
{
    if(v->size == v->capacity)
    {
        v->capacity *= 2;
        v->data = (char**)realloc(v->data, sizeof(char*) * v->capacity);
    }
    v->data[v->size++] = s;
}

vectorstring split(char *s)
{
    vectorstring v;
    v.data = (char**)malloc(sizeof(char*) * 100);
    v.size = 0;
    v.capacity = 100;
    char *p = strtok(s, " ");
    while(p != NULL)
    {
        v.data[v.size] = (char*) malloc(1 + strlen(p));
        strcpy(v.data[v.size], p);
        v.size++;
        p = strtok(NULL, " ");
    }
    return v;
}

void handlectrlc(int sig)
{
    if(childPid > 0) kill(childPid, SIGINT);
    childPid = -1;
    printf("\n");
    run();
}

void run()
{
    childPid = -1;
    char s[1000], inputfile[1000], outputfile[1000];
    printf("%s", getcwd((char*)NULL, (size_t)0));
    printf(PROMPT);
    scanf("%[^\n]s", s);
    getchar();
    if(!strcmp(s, "exit")) exit(0);
    push_back(&cmds, s);
    vectorstring v = split(s);
    
    FILE* fpin;
    if(!strcmp(v.data[0], "cd"))
    {
        chdir(v.data[1]);
        run();
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
        execvp(args[0], args);
    }
    wait(NULL);
}

int main()
{
    signal(SIGINT, handlectrlc);
    cmds.capacity = 500;
    cmds.size = 0;
    cmds.data = (char**)malloc(sizeof(char*) * 500);
    while(1)
    {
        run();
    }
    return 0;
}