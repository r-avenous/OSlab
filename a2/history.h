#ifndef __HISTORY_H__
#define __HISTORY_H__
#include "utility.h"
#include <readline/readline.h>
// #include <readline/history.h>

// const char *history_file;

int handleup(int count, int key);
int handledown(int count, int key);
void run();

#endif