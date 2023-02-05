#include "utility.hpp"

pid_t childPid;
vector<string> cmds;
int scaninterrupt = 0, background = 0;

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
void run()
{
    string s, inputfile, outputfile;
    char input[1000];
    cout.flush();
    cout << getcwd((char*)NULL, size_t(0)) << PROMPT;
    cin.clear();
    getline(cin, s);
    if(scaninterrupt) 
    {
        scaninterrupt = 0;
        return;
    }
    if(stringEmpty(s)) return;
    if(s == "exit") exit(0);
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