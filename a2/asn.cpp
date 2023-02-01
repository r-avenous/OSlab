#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

using namespace std;

#define PROMPT " |#| "

vector<string> split(string s, char delim = ' ')
{
    vector<string> v;
    stringstream ss(s);
    string item;
    while(getline(ss, item, delim))
    {
        v.push_back(item);
    }
    return v;
}
pid_t pid;
vector<string> cmds;
void run();
void handlectrlc(int sig)
{
    cout << '\n';
    run();
}
void run()
{
    string s, inputfile, outputfile;
    cout.flush();
    cout << getcwd((char*)NULL, size_t(0)) << PROMPT;
    getline(cin, s);
    if(s == "exit") exit(0);
    cmds.push_back(s);
    vector<string> v = split(s);
    FILE* fpin;
    if(v[0] == "cd")
    {
        chdir(v[1].c_str());
        run();
    }
    for(int i=0; i<v.size(); i++)
    {
        if(v[i] == "<")
        {
            inputfile = v[i+1];
            fpin = fopen(inputfile.c_str(), "r");
            i++;
        }
        else if(v[i] == ">")
        {
            outputfile = v[i+1];
            i++;
        }
    }
    if(pid = fork() == 0)
    {
        char* args[v.size() + 1];
        for(int i = 0; i < v.size(); i++)
        {
            args[i] = (char*)v[i].c_str();
        }
        args[v.size()] = NULL;
        execvp(args[0], args);
    }
    wait(NULL);
}
int main()
{
    signal(SIGINT, handlectrlc);
    while(1)
    {
        run();
    }
    return 0;
}