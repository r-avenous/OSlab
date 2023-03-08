#include "pushUpdate.hpp"
#include "helper.hpp"

extern vector<action> pushUpdateQueue;
extern pthread_mutex_t pushUpdateQueueLock;
extern pthread_cond_t pushUpdateQueueCond;
extern unordered_map<int, vector<int>> graph;
extern unordered_map<int, vector<action>> feedQueue;
extern Out out;

void* pushUpdate(void* arg)
{
    int index = *(int*)arg;
    while(1)
    {
        pthread_mutex_lock(&pushUpdateQueueLock);
        pthread_cond_wait(&pushUpdateQueueCond, &pushUpdateQueueLock);
        if(pushUpdateQueue.size() > 0)
        {
            action a = pushUpdateQueue.back();
            pushUpdateQueue.pop_back();
            out << "Push Update received at " << index << " : " << a;
            pthread_mutex_unlock(&pushUpdateQueueLock);
            for(int neighbor: graph[a.userID])
            {
            }
        }
        else pthread_mutex_unlock(&pushUpdateQueueLock);
    }
    return nullptr;
}