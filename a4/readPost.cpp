#include "readPost.hpp"
#include "helper.hpp"

extern unordered_map<int, vector<int>> graph;

void *readPost(void *arg)
{
    int index = *(int*)arg;
    Out out("RP Thread " + to_string(index) + ".txt");
}