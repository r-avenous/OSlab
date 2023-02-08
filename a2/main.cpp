#include "utility.hpp"

pid_t childPid;
vector<string> cmds;
int scaninterrupt = 0, background = 0;

void sigint_handler(int signum)
{
    scaninterrupt = 1;
    // rl_done = 1;
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
    string s, inputfile, outputfile, prompt;
    char input[1000];
    cout.flush();
    prompt = getcwd((char*)NULL, size_t(0));
    prompt += PROMPT;
    cin.clear();
    s = readline(prompt.c_str());
    if(scaninterrupt) 
    {
        scaninterrupt = 0;
        // rl_done=0;
        return;
    }
    if(stringEmpty(s)) return;
    add_history((char*)s.c_str());
    if(s == "exit") {
        write_history();
        exit(0);
    }
    cmds.push_back(s);
    vector<string> v;
    vector<pair<string,int>> pair_vec = split(s, &background);
    for(int i=0; i<pair_vec.size(); i++){
        if(pair_vec[i].second == 1){
            vector<string> temp = wildcard_handler(pair_vec[i].first);
            if(temp.size()==0){
                v.push_back(pair_vec[i].first);
            }
            for(int j=0; j<temp.size(); j++){
                v.push_back(temp[j]);
            }
        }
        else{
            v.push_back(pair_vec[i].first);
        }
    }
    /*
    int temp_fd1[2],temp_fd2[2];
    vector<string> parent_dir;
    parent_dir.clear();
    string str="";
    char buff[1000];
    parent_dir.push_back(str);
    for(int i=0; i<pair_vec.size(); i++){
        if(pair_vec[i].second == 1){
            vector<pair<string,int>> temp = wildcard(pair_vec[i].first);
            // for(int j=0; j<temp.size(); j++){
            //     v.push_back(temp[j].first);
            // }
            for(int j=0; j<temp.size(); j++){
                int parent_dir_size = parent_dir.size();
                for(int k=0; k<parent_dir_size; k++){
                    if(temp[j].second==1){
                        pipe(temp_fd1);
                        if(fork()==0){
                            close(temp_fd1[0]);
                            dup2(temp_fd1[1], 1);
                            char* args[3];
                            args[0] = (char*)malloc(3*sizeof(char));
                            strcpy(args[0], "ls");
                            if(parent_dir[k]!="")
                                args[1] = (char*)parent_dir[k].c_str();
                            else
                                args[1] = NULL;
                            // args[1] = (char*)parent_dir[k].c_str();
                            args[2] = NULL;
                            if(execvp(args[0], args) == -1)
                            {
                                cout << "Command not found" << endl;
                                exit(0);
                            }
                        }
                        close(temp_fd1[1]);
                        pipe(temp_fd2);
                        if(fork()==0){
                            close(temp_fd2[0]);
                            dup2(temp_fd1[0], 0);
                            dup2(temp_fd2[1], 1);
                            char* args[4];
                            args[0] = (char*)malloc(5*sizeof(char));
                            strcpy(args[0], "grep");
                            args[1] = (char*)malloc(3*sizeof(char));
                            strcpy(args[1], "-w");
                            args[2] = (char*)temp[j].first.c_str();
                            args[3] = NULL;
                            if(execvp(args[0], args) == -1)
                            {
                                cout << "Command not found" << endl;
                                exit(0);
                            }
                        }
                        close(temp_fd1[0]);
                        close(temp_fd2[1]);
                        memset(buff, 0, 1000);
                        read(temp_fd2[0], buff, 1000);
                        close(temp_fd2[0]);
                        // cout << buff << endl;
                        string temp_str(buff);
                        // cout << temp_str << endl;
                        for(int l=0; l<temp_str.size(); l++){
                            if(temp_str[l] == EOF)
                                break;
                            if(temp_str[l] == '\n'){
                                parent_dir.push_back(parent_dir[0] + str + "/");
                                // cout << "first" << parent_dir[0] + str + "/" << endl;
                                str="";
                            }
                            else{
                                str+=temp_str[l];
                            }
                        }
                        // cout<<"jisaag"<<endl;
                        parent_dir.erase(parent_dir.begin());
                        // for(int l=0; l<parent_dir.size(); l++){
                        //     cout << parent_dir[l] << endl;
                        // }
                    }
                    else{
                        parent_dir.push_back(parent_dir[0] + temp[j].first + "/");
                        // cout << parent_dir[0] + temp[j].first + "/" << endl;
                        parent_dir.erase(parent_dir.begin());
                    }

                }
            }
            for(int j=0; j<parent_dir.size(); j++){
                v.push_back(parent_dir[j].substr(0, parent_dir[j].size()-1));
                // cout<<parent_dir[j].substr(0, parent_dir[j].size()-1)<<endl;
            }
        }
        else{
            v.push_back(pair_vec[i].first);
        }
    }
    */
    int fd,pipe_fd[2],prev_pipe_fd[2];
    int parity = 0, initial = 0;
    int set =0;
    FILE* fpin;
    if(v[0] == "cd")
    {
        chdir(v[1].c_str());
        return;
    }

    childPid = fork();
    if(childPid == 0)
    {
        // cout << v.size() << endl << endl;
        int prev_cmd=0, flg_init=0, flg_cnt=0;
        for(int i=0; i<v.size(); i++){
            if(v[i]=="<"){
                flg_cnt = i- flg_init;
                flg_init = i + 1;
                set=1;
                inputfile = v[i+1];
                if((fd = open((char*)inputfile.c_str(),  O_RDONLY)) < 0){
                    perror("cant open file");
                    exit(0);                    
                }
                dup2(fd,0);
                close(fd);
            }
            else if(v[i]=="|"){
                if(set){
                    set = 0;
                }
                else{
                    flg_cnt = i- flg_init;
                }
                flg_init = i + 1;
                pipe(pipe_fd);
                if(parity){
                    prev_pipe_fd[1] = pipe_fd[0]; 
                }
                else{
                    prev_pipe_fd[0] = pipe_fd[0];
                }
                if(fork()==0){
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
                    // printf("\n %d %d %s\n", i, j, args[0]);
                    if(execvp(args[0], args) == -1)
                    {
                        cout << "Command not found" << endl;
                        exit(0);
                    }
                }
                wait(NULL);
                close(pipe_fd[1]);
                dup2(pipe_fd[0], 0);
                if(initial){
                    if(parity){
                        close(prev_pipe_fd[0]);
                    }
                    else{
                        close(prev_pipe_fd[1]);
                    }
                }
                else{
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
            // else if(v[i] == "&")
            // {
            //     background = 1;
            //     cout << "Background process" << background<< endl;
            // }
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


    // char* args[v.size() + 1];
    //     for(int i = 0; i < v.size(); i++)
    //     {
    //         args[i] = (char*)v[i].c_str();
    //     }
    //     args[v.size()] = NULL;
    //     if(execvp(args[0], args) < 0)
    //     {
    //         cout << "Command not found" << endl;
    //         exit(0);
    //     }


    if(!background) waitpid(childPid, NULL, 0);
    else waitpid(childPid, NULL, WNOHANG);
}

int main()
{
    initialize_readline();
    read_history();
    // open("t.txt", O_RDONLY);
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