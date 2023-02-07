#include "history.hpp"

const char *history_file = ".cmd_history";
vector<string> cmds;
FILE* fphist;

// read history from file
void read_history(){

    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fphist = fopen(history_file, "r");
 
    while ((read = getline(&line, &len, fphist)) != -1)
    {
        // added to remove newline character from getline input
        if (line[read-1] == '\n')
            line[read-1] = '\0';
        cmd_history.history.push_back(string(line));
    }

    cmd_history.size = cmd_history.history.size();
    cmd_history.index = cmd_history.size - 1;

    fclose(fphist);
    if (line)
        free(line);
}

// write deque history to file
void write_history(){

    fphist = fopen(history_file, "w");
    for (int i=0; i<cmd_history.size; i++){
        fputs(cmd_history.history[i].c_str(), fphist);
        putc('\n', fphist);
    }
    fclose(fphist);
}

// function to add terminal command to deque and file
void add_history(char* s)
{
    cmd_history.history.push_back(string(s));
    cmd_history.size++;
    cmd_history.index++;

    fphist = fopen(history_file, "a");
    printf("%d\n", fputs(s, fphist));
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

/* 
    Functions :

    Removes any text on the current terminal line
    Displays the previous history line
    If the first line is reached, the cursor stays on the first line
*/
int backward_history(int count, int key)
{
    if (cmd_history.index >= 0){

        string s;
        s = cmd_history.history[cmd_history.index];
        rl_replace_line(s.c_str(), 0);
        rl_redisplay();

        if (cmd_history.index > 0)
            cmd_history.index--;
    }
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
    if (cmd_history.index <= cmd_history.size - 1) {

        string s;
        s = cmd_history.history[cmd_history.index];
        rl_replace_line(s.c_str(), 0);
        rl_redisplay();

        if (cmd_history.index < cmd_history.size - 1)
            cmd_history.index++;
    }

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
    string s, inputfile, outputfile;
    char input[1000];
    cout.flush();
    cin.clear();

    string prompt = string(getcwd((char*)NULL, size_t(0))) + string(PROMPT);
    s = string(readline(prompt.c_str()));

    if(scaninterrupt) 
    {
        scaninterrupt = 0;
        return;
    }

    add_history((char*)s.c_str());

    if(stringEmpty(s)) return;
    if (s == "exit") {

        write_history();
        exit(0);
    }
    cmds.push_back(s);
    vector<string> v = split(s);
    
    FILE* fpin;
    if(v[0] == "cd")
    {
        chdir(v[1].c_str());
        return;
    }
    for(int i=0; i<v.size(); i++)
    {
        if(v[i] == "<")
        {
            // strcpy(inputfile, v[i+1]);
            // fpin = fopen(inputfile, "r");
            i++;
        }
        else if(v[i] == ">")
        {
            // strcpy(outputfile, v.data[i+1]);
            i++;
        }
        else if(v[i] == "&")
        {
            background = 1;
        }
    }
    childPid = fork();
    if(childPid == 0)
    {
        char* args[v.size() + 1];
        for(int i = 0; i < v.size(); i++)
        {
            args[i] = (char*)v[i].c_str();
        }
        args[v.size()] = NULL;
        if(execvp(args[0], args) < 0)
        {
            cout << "Command not found" << endl;
            exit(0);
        }
    }
    if(!background) waitpid(childPid, NULL, 0);
    else waitpid(childPid, NULL, WNOHANG);
}

int main()
{
    initialize_readline();
    read_history();

    signal(SIGINT, sigint_handler);
    signal(SIGTSTP, sigtstp_handler);
    signal(SIGCHLD, sigchld_handler);
    siginterrupt(SIGINT, 1);
    siginterrupt(SIGTSTP, 1);
    // siginterrupt(SIGCHLD, 1);
    
    while(1)
    {
        childPid = -1;
        background = 0;
        run();
    }
    return 0;
}
