#include "pushUpdate.hpp"
#include "helper.hpp"

extern vector<action> pushUpdateQueue;
extern pthread_mutex_t pushUpdateQueueLock;
extern pthread_cond_t pushUpdateQueueCond;
extern unordered_map<int, vector<int>> graph;
extern unordered_map<int, feedQueue> feedQueues;
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
            out << "PU Thread " << index << " | ";
            out << "Push Update received" << " : " << a;
            pthread_mutex_unlock(&pushUpdateQueueLock);
            for(int neighbor: graph[a.userID])
            {
                pthread_mutex_lock(&feedQueues[neighbor].lock);
                feedQueues[neighbor].push(a);
                out << "PU Thread " << index << " | ";
                out << "Push Update sent to " << neighbor << " : " << a;
                pthread_mutex_unlock(&feedQueues[neighbor].lock);
            }
        }
        else pthread_mutex_unlock(&pushUpdateQueueLock);
    }
    return nullptr;
}