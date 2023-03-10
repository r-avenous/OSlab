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
        pthread_mutex_lock(&visLock);           // lock the visited set
        Out out("RP Thread " + to_string(index) + ".log");
        
        while(visited.size() == 0)          // wait for visited to be non-empty
            pthread_cond_wait(&visCond, &visLock);
        if(visited.size() == 0)
        {                    // if visited is empty, unlock and continue
            pthread_mutex_unlock(&visLock);
            continue;
        }
        int fqchangedNode = *(visited.begin());     // get the first element of the visited set
        visited.erase(fqchangedNode);           // remove the first element from the visited set (critical section)
        pthread_mutex_unlock(&visLock);         // unlock the visited set
        out << "RP Thread " << index << " | " << "Feed Queue of " << fqchangedNode << " updated\n";
        pthread_mutex_lock(&feedQueues[fqchangedNode].lock);            // lock the feedQueue of the node
        while(!feedQueues[fqchangedNode].empty())
        {
            action a = feedQueues[fqchangedNode].pop();         // pop the top element from the feedQueue (critical section)
            out << "RP Thread " << index << " | " << "Read Post received" << " : " << a;
        }
        pthread_mutex_unlock(&feedQueues[fqchangedNode].lock);          // unlock the feedQueue of the node
    }
}