#include "utility.hpp"

pid_t childPid;
vector<string> cmds;
int scaninterrupt = 0, background = 0;

// function to handle SIGINT
void sigint_handler(int signum)
{
    if((!background) && (childPid >= 0)){
        killpg(getpgid(childPid), SIGINT);
        cout << endl;
        return;
    }

    scaninterrupt = 1;
    cout << endl;
    rl_forced_update_display();

}

// function to handle SIGTSTP
void sigtstp_handler(int signum)
{
    if(childPid == -1){

        cout << endl;
        cout << "No process to send in background" ;
        cout << endl;
        cout << getcwd((char*)NULL, size_t(0)) << PROMPT;
        return;
    }
    else
        kill(getpid(), SIGCHLD);
    
}

// function to handle SIGCHLD
void sigchld_handler(int signum)
{
    if(!background) return;
    scaninterrupt = 1;
    cout.flush();
}

void run()
{
    string s, inputfile, outputfile, prompt;
    char input[1000];
    cout.flush();
    prompt = getcwd((char*)NULL, size_t(0));
    prompt += PROMPT;
    cin.clear();
    s = readline(prompt.c_str());

    if(stringEmpty(s)) 
        return;
    
    // add command to history
    add_history((char*)s.c_str());

    if(s == "exit") {
        write_history();
        exit(0);
    }

    cmds.push_back(s);
    vector<string> v;
    vector<pair<string,int>> pair_vec = split(s, &background);

    for (int i = 0; i < pair_vec.size(); i++)
    {

        if(pair_vec[i].second == 1){

            vector<string> temp = wildcard_handler(pair_vec[i].first);
            if(temp.size() == 0)
            {
                v.push_back(pair_vec[i].first);
            }
            for(int j = 0; j<temp.size(); j++){
                v.push_back(temp[j]);
            }
        }
        else{
            v.push_back(pair_vec[i].first);
        }
    }

    int fd, pipe_fd[2], prev_pipe_fd[2];
    int parity = 0, initial = 0;
    int set = 0;
    FILE* fpin;

    if(v[0] == "cd")
    {
        chdir(v[1].c_str());
        return;
    }

    if (v[0] == "delep"){

        char* filename = (char*)v[1].c_str();
        delep(filename);
        return;
    }

    if (v[0] == "sb"){

        int argc = v.size();
        char* argv[argc];

        for(int i = 0; i < argc; i++){
            argv[i] = (char*)v[i].c_str();
        }

        sb(argc, argv);
        return;
    }

    childPid = fork();
    if(childPid == 0)
    {
        int prev_cmd = 0, flg_init = 0, flg_cnt = 0;
        for(int i = 0; i < v.size(); i++)
        {
            if(v[i] == "<")
            {
                flg_cnt = i - flg_init;
                flg_init = i + 1;
                set = 1;
                inputfile = v[i+1];

                if((fd = open((char*)inputfile.c_str(),  O_RDONLY)) < 0){
                    perror("Can't open file");
                    exit(0);                    
                }

                // replace stdin with fd of input file (for input redirection)
                dup2(fd,0);
                close(fd);
            }

            else if(v[i]=="|"){

                if(set){
                    set = 0;
                }
                else{
                    flg_cnt = i - flg_init;
                }

                flg_init = i + 1;
                pipe(pipe_fd);

                if(parity){
                    prev_pipe_fd[1] = pipe_fd[0]; 
                }
                else
                {
                    prev_pipe_fd[0] = pipe_fd[0];
                }

                if(fork() == 0)
                {
                    close(pipe_fd[0]);
                    dup2(pipe_fd[1], 1);
                    char* args[v.size() + 1];
                    int j;

                    for(j = prev_cmd; j < (prev_cmd+flg_cnt); j++)
                    {
                        if(v[j] == "<" || v[j]==">") break;
                        args[j-prev_cmd] = (char*)v[j].c_str();
                    }
                    args[j-prev_cmd] = NULL;
                    if(execvp(args[0], args) == -1)
                    {
                        cout << "Command not found" << endl;
                        exit(0);
                    }
                }

                wait(NULL);
                close(pipe_fd[1]);
                dup2(pipe_fd[0], 0);

                if(initial)
                {
                    if(parity)
                    {
                        close(prev_pipe_fd[0]);
                    }
                    else
                    {
                        close(prev_pipe_fd[1]);
                    }
                }
                else
                {
                    initial = 1;
                }
                parity = 1 - parity;
                prev_cmd = i+1;
            }

            else if(v[i]==">"){
                flg_cnt = i- flg_init;
                flg_init = i + 1;
                outputfile = v[i+1];
                fd = open((char*)outputfile.c_str() , O_RDWR | O_CREAT, 0666);
                dup2(fd, 1);
                close(fd);
            }
        }

        char* args[v.size() + 1];
        int j;
        for(j = prev_cmd; j < v.size(); j++)
        {
            if((v[j] == "<") || (v[j] == ">") || (v[j]=="&")) break;
            args[j-prev_cmd] = (char*)v[j].c_str();
        }
        args[j-prev_cmd] = NULL;
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
    
    while(1)
    {
        childPid = -1;
        background = 0;
        run();
    }
    return 0;
}