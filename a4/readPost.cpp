#include "readPost.hpp"
#include "helper.hpp"

extern unordered_map<int, vector<int>> graph;
extern unordered_map<int, vector<action>> feedQueue;

void *readPost(void *arg)
{
    int index = *(int*)arg;
}