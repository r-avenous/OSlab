#ifndef __MAIN_H__
#define __MAIN_H__

#include <bits/stdc++.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <dirent.h>
#include <limits.h>
#include <fstream>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <fnmatch.h>
#define HZ sysconf(_SC_CLK_TCK)
#define MAX_DEPTH 5

using namespace std;

#define PROMPT " [#] "
#define SIZE 1000

typedef struct _history_state{
    int index;
    int size;
    deque<string> history;
} history_state;

// functions for string manipulations
bool stringEmpty(string s);
bool is_number(string s);
vector<pair<string,int>> split(string s, int *background);

// functions for wildcard manipulations
vector<pair<string,int>> wildcard_split(string s);
vector<string> wildcard_handler(string s);

// functions for managing terminal history 
void read_history();
void write_history();
void add_history(char* s);
int backward_history(int count, int key);
int forward_history(int count, int key);
void initialize_readline();

// functions for malware detection
int count_children(const pid_t pid);
float time_taken(const pid_t pid);
float cpu_usage(const pid_t pid);
int find_avg_cpu_of_child(const pid_t pid, int depth);
float heuristic(const pid_t pid);
pid_t get_parent(const pid_t pid);
pid_t suggestMalware(pid_t pid);
void traverse(pid_t pid, int gen);
void sb(int argc, char *argv[]);

// functions for delete without prejudice (delep)
void get_process_open_lock_file(char* filename, vector<int>* open_pids);
void kill_processes(vector<int> pids);
void delep(char* file);

#endif

