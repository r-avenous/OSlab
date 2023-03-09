#include "pushUpdate.hpp"
#include "helper.hpp"
#include <unordered_set>

extern vector<action> pushUpdateQueue;
extern pthread_mutex_t pushUpdateQueueLock;
extern pthread_cond_t pushUpdateQueueCond;
extern unordered_map<int, vector<int>> graph;
extern unordered_map<int, feedQueue> feedQueues;
extern Out out;

unordered_set<int> visited;
pthread_mutex_t visLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t visCond = PTHREAD_COND_INITIALIZER;

void* pushUpdate(void* arg)
{
    int index = *(int*)arg;
    
    while(1)
    {
        pthread_mutex_lock(&pushUpdateQueueLock);
        
        while(pushUpdateQueue.empty())
            pthread_cond_wait(&pushUpdateQueueCond, &pushUpdateQueueLock);

        Out out("PU Thread " + to_string(index) + ".txt");
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
                out << "Push Update sent to " << neighbor << " : " << feedQueues[neighbor].top();
                pthread_mutex_unlock(&feedQueues[neighbor].lock);
                
                pthread_mutex_lock(&visLock);
                visited.insert(neighbor);
                pthread_cond_broadcast(&visCond);
                pthread_mutex_unlock(&visLock);
            }
        }
        else pthread_mutex_unlock(&pushUpdateQueueLock);
    }
    return nullptr;
}