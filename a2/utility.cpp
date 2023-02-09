#include "utility.hpp"

// variables for history
FILE* fphist;
string last_cmd;
int counter = 0;
history_state cmd_history;
const char *history_file = ".cmd_history";

// variables for delep
const char *proc_path = "/proc/";
const char *lock_file = "/proc/locks";

// function to check if string is empty
bool stringEmpty(string s)
{
    for(char c: s)
    {
        if(c != ' ' && c != '\t' && c != '\n') return false;
    }
    return true;
}

// function to check if string is a number
bool is_number(string s)
{
    auto it = s.begin();
    for(;(it!=s.end()) && (isdigit(*it)); it++){

    }
    return !s.empty() && it == s.end();
}

// function to split the command into tokens
vector<pair<string,int>> split(string s1, int* background)
{
    vector<pair<string,int>> v;
    string s = "";
    int wildcard = 0;
    pair<string,int> p;

    for(int i = 0; i < s1.size(); i++)
    {
        if(s1[i]==' ') continue;
        while(s1[i] != '|' && s1[i] != '<' && s1[i] != '>' && s1[i] != '&' && s1[i] != ' ' && (i<s1.size())) 
        {
            if(s1[i] == '\"'){
                i++;
                while(s1[i] != '\"' && (i<s1.size())){
                    s += s1[i];
                    i++;
                }
                i++;
                if(s1[i] == ' ' || s1[i] == '|' || s1[i] == '<' || s1[i] == '>' || s1[i] == '&'){
                    p = make_pair(s,wildcard);
                    v.push_back(p);
                    s="";
                    i--;
                    break;
                }
            }
            if(s1[i] == '\''){
                i++;
                while(s1[i] != '\'' && (i<s1.size())){
                    s += s1[i];
                    i++;
                }
                i++;
                if(s1[i] == ' ' || s1[i] == '|' || s1[i] == '<' || s1[i] == '>' || s1[i] == '&'){
                    p = make_pair(s,wildcard);
                    v.push_back(p);
                    s="";
                    i--;
                    break;
                }
            }
            if(((s1[i] == '*') || (s1[i] == '?')) && (s1[i-1] != '\\')){
                wildcard = 1;
            }
            s += s1[i];
            i++;
        }
        if(s!=""){
            p = make_pair(s,wildcard);
            v.push_back(p);
            wildcard = 0;
            s="";
        }
        if((s1[i] == '|' || s1[i] == '<' || s1[i] == '>' || s1[i] == '&') && (i<s1.size())){
            if(s1[i] == '&'){
                *background = 1;
            }
            s+=s1[i];
            p = make_pair(s,0);
            v.push_back(p);
        }
        s="";
        wildcard = 0;
    }

    return v;
}

// function to split the wildcard command into tokens
vector<pair<string,int>> wildcard_split(string s)
{
    int count = 0;
    for(int i=0; i<s.size(); i++){
        if(s[i] == '/'){
            count++;
        }
    }

    vector<pair<string,int>> v;
    pair<string,int> p;
    string str="";
    int wildcard = 0;
    p = make_pair(str,wildcard);

    for(int i=0; i<s.size(); i++){
        if(s[i]=='/'){
            count--;
            p.first = str;
            p.second = wildcard;
            v.push_back(p);
            str="";
            wildcard = 0;
        }
        else{
            if(s[i] == '*'){
                wildcard = 1;
            }
            else if(s[i] == '?'){
                wildcard = 1;
            }
            str+=s[i];
        }
    }

    p.first = str;
    p.second = wildcard;
    v.push_back(p);
    return v;
}

// function to handle wildcard commands
vector<string> wildcard_handler(string s)
{
    vector<pair<string,int>> v = wildcard_split(s);
    vector<string> result;
    string p = "";
    result.push_back(p);
    p="";
    DIR *dir;
    for(int i=0; i<v.size(); i++){
        int size = result.size();
        for(int j=0; j<size; j++)
        {
            if(v[i].second == 1)
            {
                if(result[0]==""){
                    dir = opendir (".");
                }
                else{
                    dir = opendir (result[0].c_str());
                }
                if(dir != NULL) {
                    struct dirent *ent;
                    while ((ent = readdir (dir)) != NULL) {
                        if(fnmatch(v[i].first.c_str(), ent->d_name, FNM_CASEFOLD) == 0){
                            p = ent->d_name;
                            result.push_back(result[0] + p + "/");
                        }
                    }
                    result.erase(result.begin());
                    closedir (dir);
                } else {
                    perror ("");
                }
            }
            else{
                result.push_back(result[0] + v[i].first + "/");
                result.erase(result.begin());
            }
        }
    }
    for(int i=0; i<result.size(); i++){
        result[i] = result[i].substr(0,result[i].size()-1);
    }
    return result;
}

// read history from file
void read_history() {

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
    cmd_history.index = cmd_history.size;

    fphist = fopen(history_file, "a");
    printf("%d\n", fputs(s, fphist));
    putc('\n', fphist);
    fclose(fphist);

    if (cmd_history.size > SIZE)
    {
        cmd_history.history.pop_front();
        cmd_history.size--;
        cmd_history.index--;
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
    counter--;
    if(counter==-1){
        last_cmd = string(rl_line_buffer);
    }
    if (cmd_history.index > 0)
        cmd_history.index--;
    
    if (cmd_history.index >= 0){

        string s;
        s = cmd_history.history[cmd_history.index];
        rl_replace_line(s.c_str(), 0);
        rl_redisplay();
        rl_point = rl_end;
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
    if(counter<0){
        counter++;
    }
    if (cmd_history.index < (cmd_history.size - 1)){
        cmd_history.index++;
        string s;
        s = cmd_history.history[cmd_history.index];
        rl_replace_line(s.c_str(), 0);
        rl_redisplay();
        rl_point = rl_end;
    }
    else{
        if(cmd_history.index < cmd_history.size){
            cmd_history.index++;
        }
        rl_replace_line(last_cmd.c_str(), 0);
        rl_redisplay();
        rl_point = rl_end;
    }
    return 0;
}

// initialize readline settings by mapping functions to keys
void initialize_readline(){

    // map backward_history function to up arrow key
    rl_bind_keyseq("\e[A", backward_history);

    // map forward_history function to down arrow key
    rl_bind_keyseq("\e[B", forward_history);

}

// function to count number of children of a process
int count_children(const pid_t pid) 
{
  DIR* proc_dir;
  int num_children = 0;

  if ((proc_dir = opendir("/proc"))) 
  {
    for (struct dirent* proc_id; (proc_id = readdir(proc_dir))!=NULL;) {
      if (is_number(proc_id->d_name)) 
      {
        ifstream ifs(string("/proc/" + string(proc_id->d_name) + "/stat").c_str());
        string parent;
        for (int i = 0; i < 4; ++i) {
          ifs >> parent;
        }

        if (parent == to_string(pid)) {
          ++num_children;
        }
      }
    }
    closedir(proc_dir);
    return num_children;
  }
  perror("could not open directory");
  return -1;
}

// function to find time taken by a process
float time_taken(const pid_t pid)
{
  ifstream ifs(string("/proc/" + to_string(pid) + "/stat").c_str());
  string process_start, up_time;
  for(int i=0; i<22; i++){
    ifs >> process_start;
  }

  ifs.close();
  ifs.open(string("/proc/uptime").c_str());
  ifs >> up_time;

  float time_taken = stof(up_time) - (stof(process_start))/HZ;
  return time_taken;
}

// function to find cpu usage of a process
float cpu_usage(const pid_t pid)
{
  ifstream ifs(string("/proc/" + to_string(pid) + "/stat").c_str());
  string u_time, s_time, temp;
  float process_elapsed = time_taken(pid);
  for(int i=0; i<22; i++){
    if(i==13){
      ifs >> u_time;
    }
    else if(i == 14){
      ifs >> s_time;
    }
    else{
      ifs >> temp;
    }
  }
  
  float process_usage = stof(u_time)/HZ + stof(s_time)/HZ;
  return ((process_usage * 100)/ process_elapsed);
}

// function to find average cpu usage of children
int find_avg_cpu_of_child(const pid_t pid, int depth)
{
  if(depth == MAX_DEPTH){
    return 0;
  }

  float first_gen = 0, num_children = 0;
  DIR* proc_dir;
  map<int,int> child_count;

  if((proc_dir = opendir("/proc")))
  {
    struct dirent* proc_id;
     for (proc_id; (proc_id = readdir(proc_dir));)
     {
      if (is_number(proc_id->d_name))
      {
        ifstream ifs(string("/proc/" + string(proc_id->d_name) + "/stat").c_str());
        string parent, status;
        for(int i=0; i<4; i++){
          if(i==2){
            ifs >> status;
          }
          ifs >> parent;
        }
        if(parent == to_string(pid))
        {
          first_gen = cpu_usage(stoi(proc_id->d_name));
          float avg = find_avg_cpu_of_child(stoi(proc_id->d_name), depth+1);
          if(avg != 0){
            first_gen += avg/ count_children(stoi(proc_id->d_name));
          }
          num_children++;
        }
      }
    }
  }
  closedir(proc_dir);
  return first_gen;
}

// function to find heuristic value of a process
float heuristic(const pid_t pid){
  float heuristic = 0;
  heuristic = count_children(pid)*60/time_taken(pid) + cpu_usage(pid) + find_avg_cpu_of_child(pid,0);
  return heuristic;
}

// function to find parent of a process
pid_t get_parent(const pid_t pid)
{
  ifstream ifs(string("/proc/" + to_string(pid) + "/stat").c_str());
  string parent;
  for(int i=0; i < 4; i++)
  {
    ifs >> parent;
  }
  ifs.close();
  return stoi(parent);
}

// Function to suggest the malware process
pid_t suggestMalware(pid_t pid) {

  // Check the time spent and number of children of each process
  if (get_parent(pid) == 1){
    return pid;
  }

  pid_t parent_pid = get_parent(pid);
  float par_h = heuristic(parent_pid), h = heuristic(pid);
  cout << "PARENT: "<< parent_pid << " Heuristic: " << par_h << "\n" << "CHILD: " << pid << " Heuristic: " << h <<"\n\n";
  if(par_h > h){
    suggestMalware(parent_pid);
  }
  else{
    return pid;
  }
}

// Function to traverse the process tree
void traverse(pid_t pid, int gen){
  if(pid == 1){
    cout << "No more parent process last process printed was init process\n";
    return;
  }
  pid_t parent_pid = get_parent(pid);
  cout << "Process ID: " << parent_pid << " Parent Generation " << gen <<"\n";
  traverse(parent_pid, gen+1);
}

// function to squash bug
void sb(int argc, char *argv[]) {
  
  if(fork() == 0)
  {
    pid_t pid;

    // Check if user provided a process ID
    if (argc < 2) {
      cout << "Please provide a process ID." << endl;
    }

    // Check if user used the "-suggest" flag
    if (argc == 3 && string(argv[2]) == "-suggest") 
    {
      // Get the process ID from the user
      pid = atoi(argv[1]);
      cout <<"Children: "  << count_children(pid) << "\n";
      cout << "cpu_usage: " << cpu_usage(pid) <<"\n";

      // Use the suggestMalware function to suggest the malware process
      cout << "Current Process ID: " << pid << "\n";
      traverse(pid, 1);
      pid_t malware = suggestMalware(pid);
      cout << "The expected malware Process ID is: " << malware << "\n";
      exit(0);
    } 
    else {
      // Get the process ID from the user
      pid = atoi(argv[1]);
      cout << "Children: " << count_children(pid) << "\n";

      // Use the traverse function to display the parent, grandparent, and so on of the given process
      cout << "Current Process ID: " << pid << "\n";
      traverse(pid,1);
      exit(0);
    }
    exit(0);
  }
  wait(NULL);
}

// function to get pid of the process that has the lock file open
void get_process_open_lock_file(char* filename, vector<int>* open_pids){

    struct dirent *entry;
    DIR *dp;
    int pid;

    dp = opendir(proc_path);
    if (dp == NULL) {
        perror("opendir: Path does not exist or could not be read.");
        return;
    }

    // find all pids which have opened the file
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

        while ((fd_entry = readdir(fd_dp))){

            memset(buf, '\0', 1024);
            int fd = atoi(fd_entry->d_name);

            readlink((string(fd_path) + string(fd_entry->d_name)).c_str(), buf, 1024);
            if (strstr(buf, filename) != NULL){

                found = 1;
                break;
            }
        }
        if (found){
            open_pids->push_back(pid);
            // cout << pid << endl;
        }
        closedir(fd_dp);
    }
    closedir(dp);

    // find all pids which have locked the file
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

        // find all pids which have locked file, replace them with -pid (post-processing done later)
        if (find(open_pids->begin(), open_pids->end(), pid) != open_pids->end())
            replace(open_pids->begin(), open_pids->end(), pid, -pid);
    }
}

// function to kill all processes that have the lock file open
void kill_processes(vector<int> pids){

    for (int i = 0; i < pids.size(); i++){
        kill(pids[i], SIGKILL);
        cout << "Killed process " << pids[i] << endl;
    }
}

// function to delete without prejudice
void delep(char* file)
{
    char* filename = file;
    pid_t pfd[2];

    // pipe the pfd array
    pipe(pfd);
    pid_t pid = fork();

    if (pid == 0){

        vector<int> pids;
        get_process_open_lock_file(filename, &pids);

        size_t len;
        len = pids.size();

        // write pid vector to parent process using pipe
        write(pfd[1], &len, sizeof(len));

        int p;
        for (int i = 0; i < len; i++)
        {
            p = pids[i];
            write(pfd[1], &p, sizeof(pid_t));
        }

        exit(0);
    }
    wait(NULL);

    size_t open_len;
    int p_;
    vector<int> open_pids;
    vector<int> locked_pids;

    // read list of pids from child process
    read(pfd[0], &open_len, sizeof(open_len));

    for (int i = 0; i < open_len; i++){

        read(pfd[0], &p_, sizeof(pid_t));
        if (p_ < 0)
        {
            open_pids.push_back(-p_);
            locked_pids.push_back(-p_);
        }
        else
            open_pids.push_back(p_);
    }

    cout << "PIDs which have opened the file:" << endl << endl;
    for (int i = 0; i < open_pids.size(); i++)
        cout << open_pids[i] << endl;

    cout << "PIDs which have locked the file:" << endl << endl;
    for (int i = 0; i < locked_pids.size(); i++)
        cout << locked_pids[i] << endl;

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
}
