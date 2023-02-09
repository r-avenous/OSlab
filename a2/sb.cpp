#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <map>
#define MAX_DEPTH 5
#include <dirent.h>
#include <unistd.h>
#include <limits.h>
#define HZ sysconf(_SC_CLK_TCK)
#include <string>
#include <fstream>

using namespace std;

bool is_number(string s)
{
    auto it = s.begin();
    for(;(it!=s.end()) && (isdigit(*it)); it++){

    }
    // if (isdigit(atoi(line.c_str())))
    //     return true;

    // return false;
    return !s.empty() && it == s.end();
}

int count_children(const pid_t pid) {
  DIR* proc_dir;
  int num_children = 0;
  if ((proc_dir = opendir("/proc"))) {
    //struct dirent* proc_id;
    for (struct dirent* proc_id; (proc_id = readdir(proc_dir))!=NULL;) {
      if (is_number(proc_id->d_name)) {
        //cout << proc_id->d_name << "\n";
        ifstream ifs(string("/proc/" + string(proc_id->d_name) + "/stat").c_str());
        //cout << string("/proc/" + string(proc_id->d_name) + "/stat");
        string parent;
        for (int i = 0; i < 4; ++i) {
          ifs >> parent;
          //cout << parent <<"\n";
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

float time_taken(const pid_t pid){
  ifstream ifs(string("/proc/" + to_string(pid) + "/stat").c_str());
  string process_start, up_time;
  for(int i=0; i<22; i++){
    ifs >> process_start;
  }
  ifs.close();
  ifs.open(string("/proc/uptime").c_str());
  ifs >> up_time;
  //cout << up_time <<"\n";

  float time_taken = stof(up_time) - (stof(process_start))/HZ;
  return time_taken;
}

float cpu_usage(const pid_t pid){
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
  //cout << "cpu usage tinings: " << u_time << " " << s_time <<"\n";
  float process_usage = stof(u_time)/HZ + stof(s_time)/HZ;
  return ((process_usage * 100)/ process_elapsed);
}

int find_avg_cpu_of_child(const pid_t pid, int depth){
  //cout << "PID: " << pid <<"\n";
  if(depth == MAX_DEPTH){
    return 0;
  }
  float first_gen = 0, num_children = 0;
  DIR* proc_dir;
  map<int,int> child_count;
  if((proc_dir = opendir("/proc"))){
    struct dirent* proc_id;
     for (proc_id; (proc_id = readdir(proc_dir));){
      if(is_number(proc_id->d_name)){
        ifstream ifs(string("/proc/" + string(proc_id->d_name) + "/stat").c_str());
        string parent, status;
        for(int i=0; i<4; i++){
          if(i==2){
            ifs >> status;
          }
          ifs >> parent;
        }
        if(parent == to_string(pid)){
          //cout << "END\n";
          first_gen = cpu_usage(stoi(proc_id->d_name)) + find_avg_cpu_of_child(stoi(proc_id->d_name), depth+1)/(count_children(stoi(proc_id->d_name))+1);
          num_children++;
        }
      }
    }
  }
  //cout << "PID: " << pid <<"\n";
  closedir(proc_dir);
  return first_gen;
}

float heuristic(const pid_t pid){
  float heuristic = 0;
  heuristic = count_children(pid);
  //cout << "HEU 1: " << heuristic <<"\n";
  heuristic += cpu_usage(pid);
  //cout <<"HEU 2: " << heuristic <<"\n";
  heuristic += find_avg_cpu_of_child(pid,0);
  return heuristic;
}

pid_t get_parent(const pid_t pid){
  ifstream ifs(string("/proc/" + to_string(pid) + "/stat").c_str());
  string parent;
  for(int i=0; i<4; i++){
    ifs >> parent;
    //cout << parent <<"\n";
  }
  //cout << parent <<"\n";
  ifs.close();
  return stoi(parent);
}

// Function to suggest the malware process
pid_t suggestMalware(pid_t pid) {
  // Check the time spent and number of children of each process
  //cout << pid << "\n";
  if (get_parent(pid) == 1){
    return pid;
  }
  pid_t parent_pid = get_parent(pid);
  //cout << "Parent: " << parent_pid <<"\n";
  float par_h = heuristic(parent_pid), h = heuristic(pid);
  cout << "PARENT: "<< parent_pid << " Heuristic: " << par_h << "\n" << "CHILD: " << pid << " Heuristic: " << h <<"\n\n";
  if(par_h > h){
    suggestMalware(parent_pid);
  }
  else{
    cout << "Parent: " << parent_pid <<"\n";
    return pid;
  }
}

void traverse(pid_t pid, int gen){
  if(pid == 1){
    cout << "No more parent process last process printed was init process\n";
    return;
  }
  pid_t parent_pid = get_parent(pid);
  cout << "Process ID: " << parent_pid << " Parent Generation " << gen <<"\n";
  traverse(parent_pid, gen+1);
}
int main(int argc, char *argv[]) {
  //cout << count_children(atoi(argv[1]));
  pid_t pid;
  // Check if user provided a process ID
  if (argc < 2) {
    cout << "Please provide a process ID." << endl;
    return 1;
  }
  // Check if user used the "-suggest" flag
  if (argc == 3 && string(argv[2]) == "-suggest") {
    // Get the process ID from the user
    pid = atoi(argv[1]);
    cout <<"Children: "  << count_children(pid) << "\n";
    cout << "cpu_usage: " << cpu_usage(pid) <<"\n";
    // Use the suggestMalware function to suggest the malware process
    cout << "Current Process ID: " << pid << "\n";
    traverse(pid, 1);
    pid_t malware = suggestMalware(pid);
    cout << "The expected malware Process ID is: " << malware << "\n";
  } 
  else {
    // Get the process ID from the user
    pid = atoi(argv[1]);
    cout << "Children: " << count_children(pid) << "\n";
    // Use the traverse function to display the parent, grandparent, and so on of the given process
    cout << "Current Process ID: " << pid << "\n";
    traverse(pid,1);
  }
  return 0;
}
