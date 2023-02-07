#include "history.h"

const char *history_file = ".cmd_history";
pid_t childPid;
vectorstring cmds;
FILE* fphist;
history_state cmd_history;

// read history from file
void read_history(){

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fphist = fopen(history_file, "r");
 
    while ((read = getline(&line, &len, fphist)) != -1) 
        cmd_history.history.push_back(string(line));

    cmd_history.size = cmd_history.history.size();
    cmd_history.index = cmd_history.size - 1;

    fclose(fphist);
    if (line)
        free(line);
}

// write deque history to file
void write_history(){

    fphist = fopen(history_file, "w");
    for (int i=0; i<cmd_history.size; i++)
        fputs(cmd_history.history[i].c_str(), fphist);
    fclose(fphist);
}

// function to add terminal command to deque and file
void add_history(char* s)
{
    cmd_history.history.push_back(string(s));
    cmd_history.size++;
    cmd_history.index++;
    fphist = fopen(history_file, "a");
    fputs(s, fphist);
    putc('\n', fphist);
    fclose(fphist);

    if (cmd_history.size > SIZE)
    {
        cmd_history.history.pop_front();
        cmd_history.size--;
        cmd_history.index--;

        // remove first line from history file
        write_history();
    }
}

void tail(FILE* in, int n)
{
    int count = 0;
  
    unsigned long long pos;
    char str[SIZE];
  
    if (fseek(in, 0, SEEK_END))
        perror("fseek() failed");

    else
    {
        pos = ftell(in);
  
        // search for '\n' characters
        while (pos)
        {
            // Move 'pos' away from end of file.
            if (!fseek(in, --pos, SEEK_SET))
            {
                if (fgetc(in) == '\n')
  
                    // stop reading when n newlines
                    // is found
                    if (count++ == n)
                        break;
            }
            else
                perror("fseek() failed");
        }

        // print last n lines
        printf("Printing last %d lines -\n", n);
        while (fgets(str, sizeof(str), in))
            printf("%s", str);
    }
    printf("\n\n");
}

/* 
    Functions :

    Removes any text on the current terminal line
    Displays the previous history line
    If the first line is reached, the cursor stays on the first line
*/
int backward_history(int count, int key)
{
    printf("up");
    // fseek(fphist, -1, SEEK_END);
    return 0;
}

/* 
    Functions :

    Removes any text on the current terminal line
    Displays the next history line
    If the last line is reached, the cursor stays on the last line
*/
int forward_history(int count, int key)
{
    printf("down");
    return 0;
}

void initialize_readline(){

    // map backward_history function to up arrow key
    rl_bind_keyseq("\e[A", backward_history);

    // map forward_history function to down arrow key
    rl_bind_keyseq("\e[B", forward_history);

}

void run()
{
    childPid = -1;
    char *s, *inputfile, *outputfile;
    
    s = (char *)malloc(SIZE * sizeof(char));
    inputfile = (char *)malloc(SIZE * sizeof(char));
    outputfile = (char *)malloc(SIZE * sizeof(char));

    s = readline(PROMPT);

    printf("Line : %s\n", rl_line_buffer);

    // add entered commands to history deque
    add_history(s);

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
    read_history();
    cmds.capacity = 500;
    cmds.size = 0;
    cmds.data = (char**)malloc(sizeof(char*) * 500);

    while (1){

        run();
    }
    return 0;
}