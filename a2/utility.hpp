#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

using namespace std;

#define PROMPT " [#] "

vector<string> split(string s);
bool stringEmpty(string s);
void run();

#endif
