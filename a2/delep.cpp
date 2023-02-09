#include "utility.hpp"

pid_t childPid;
vector<string> cmds;
int scaninterrupt = 0, background = 0;
const char *proc_path = "/proc/";
const char *lock_file = "/proc/locks";

void get_process_open_lock_file(char* filename, vector<int>* open_pids){

    struct dirent *entry;
    DIR *dp;
    int pid;

    dp = opendir(proc_path);
    if (dp == NULL) {
        perror("opendir: Path does not exist or could not be read.");
        return;
    }

    // print all pids which have opened the file
    cout << "PIDs which have opened the file:" << endl << endl;
    while ((entry = readdir(dp))){

        if ((pid = atoi(entry->d_name)) == 0) 
            continue;

        string fd_path = string(proc_path) + string(entry->d_name) + string("/fd/");
        struct dirent *fd_entry;
        DIR *fd_dp;

        fd_dp = opendir(fd_path.c_str());
        if (fd_dp == NULL)
            continue;
        
        char buf[1024];
        short found = 0;
        // struct stat *stat_buf = new struct stat;

        while ((fd_entry = readdir(fd_dp))){

            memset(buf, '\0', 1024);
            int fd = atoi(fd_entry->d_name);
            // fstat(fd, stat_buf);

            readlink((string(fd_path) + string(fd_entry->d_name)).c_str(), buf, 1024);
            if (strstr(buf, filename) != NULL){

                found = 1;
                break;
            }
        }
        if (found){
            open_pids->push_back(pid);
            cout << pid << endl;
        }
        closedir(fd_dp);
    }
    closedir(dp);

    // print all pids which have locked the file
    cout << endl << "PIDs which have locked the file:" << endl << endl;
    FILE *fp = fopen(lock_file, "r");
    if (fp == NULL){
        perror("fopen: Path does not exist or could not be read.");
        return;
    }

    char line[1024];
    while (fgets(line, 1024, fp) != NULL){

        char *token = strtok(line, " ");
        int i = 0;
        while (token != NULL){
            if (i == 4){
                pid = atoi(token);
                break;
            }
            token = strtok(NULL, " ");
            i++;
        }

        if (find(open_pids->begin(), open_pids->end(), pid) != open_pids->end()){
            cout << pid << endl;
        }
    }
}

void kill_processes(vector<int> pids){

    cout << "Inside kill_processes" << endl;
    for (int i = 0; i < pids.size(); i++){
        kill(pids[i], SIGTERM);
        cout << "Killed process " << pids[i] << endl;
    }
}

int main(int args, char* argv[])
{
    char* filename = argv[1];
    pid_t pfd[2];

    // pipe the pfd array
    pipe(pfd);
    pid_t pid = fork();

    if (pid == 0){

        vector<int> pids;
        get_process_open_lock_file(filename, &pids);

        // cout << "#####################" << endl;
        size_t len;
        // cout << "!!###################" << endl;
        // cout << &pids << endl;
        len = pids.size();
        // cout << "Len child : " << len << endl;

        // write pid vector to parent process using pipe
        write(pfd[1], &len, sizeof(len));
        // cout << "!!###################" << endl;
        int p;

        for (int i = 0; i < len; i++)
        {
            // cout << "!####################" << endl;
            p = pids[i];
            // cout << p << endl;
            // cout << "#####################" << endl;
            write(pfd[1], &p, sizeof(pid_t));
        }
        // write(pfd[1], &pids[0], pids.size() * sizeof(pid_t));
        // cout << "!!!!################" << endl;

        exit(0);
    }
    wait(NULL);
    // cout << "!!!!##############!" << endl;

    size_t open_len;
    int p_;
    vector<int> open_pids;

    // read list of pids from child process
    read(pfd[0], &open_len, sizeof(open_len));
    // cout << "!!!!##############!!" << endl;

    for (int i = 0; i < open_len; i++){

        read(pfd[0], &p_, sizeof(pid_t));
        open_pids.push_back(p_);
    }

    cout << "Do you want to kill these processes? (y/n): ";
    char c;
    cin >> c;

    if (c == 'y'){

        cout << "Killing processes..."<< endl;
        kill_processes(open_pids);
        if (remove(filename) == 0)
            cout << "File deleted successfully" << endl;
        else
            cout << "Error: unable to delete the file" << endl;
    }

    return 0;
}