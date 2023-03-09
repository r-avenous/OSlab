#include "readPost.hpp"
#include "helper.hpp"
#include <unordered_set>

extern unordered_map<int, vector<int>> graph;
extern unordered_map<int, feedQueue> feedQueues;
extern Out out;
extern unordered_set<int> visited;
extern pthread_mutex_t visLock;
extern pthread_cond_t visCond;

void *readPost(void *arg)
{
    int index = *(int*)arg;
    while(1)
    {
        pthread_mutex_lock(&visLock);
        Out out("RP Thread " + to_string(index) + ".txt");
        
        while(visited.size() == 0)
            pthread_cond_wait(&visCond, &visLock);
        if(visited.size() == 0)
        {
            pthread_mutex_unlock(&visLock);
            continue;
        }
        int fqchangedNode = *(visited.begin());
        visited.erase(fqchangedNode);
        pthread_mutex_unlock(&visLock);
        out << "RP Thread " << index << " | " << "Feed Queue of " << fqchangedNode << " updated\n";
        pthread_mutex_lock(&feedQueues[fqchangedNode].lock);
        while(!feedQueues[fqchangedNode].empty())
        {
            action a = feedQueues[fqchangedNode].pop();
            out << "RP Thread " << index << " | " << "Read Post received" << " : " << a;
        }
        pthread_mutex_unlock(&feedQueues[fqchangedNode].lock);
    }
}