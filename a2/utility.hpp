#ifndef __MAIN_H__
#define __MAIN_H__

#include <bits/stdc++.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <readline/readline.h>
#include <fnmatch.h>

using namespace std;

#define PROMPT " [#] "
#define SIZE 1000

typedef struct _history_state{
    int index;
    int size;
    deque<string> history;
} history_state;

void read_history();
void write_history();
void add_history(char* s);
int backward_history(int count, int key);
int forward_history(int count, int key);
void initialize_readline();

vector<pair<string,int>> split(string s, int *background);
vector<pair<string,int>> wildcard_split(string s);
vector<string> wildcard_handler(string s);
bool stringEmpty(string s);
void run();

#endif

