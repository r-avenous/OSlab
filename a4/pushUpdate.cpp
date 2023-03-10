#include "pushUpdate.hpp"
#include "helper.hpp"
#include <unordered_set>

extern vector<action> pushUpdateQueue;
extern pthread_mutex_t pushUpdateQueueLock;
extern pthread_cond_t pushUpdateQueueCond;
extern unordered_map<int, vector<int>> graph;
extern unordered_map<int, feedQueue> feedQueues;
extern Out out;

unordered_set<int> visited;             // set of nodes whose feed queues have been updated
pthread_mutex_t visLock = PTHREAD_MUTEX_INITIALIZER;            // lock for visited
pthread_cond_t visCond = PTHREAD_COND_INITIALIZER;              // condition variable for visited

void* pushUpdate(void* arg)
{
    int index = *(int*)arg;
    
    while(1)
    {
        pthread_mutex_lock(&pushUpdateQueueLock);           // lock the pushUpdateQueue
        Out out("PU Thread " + to_string(index) + ".log");      
        
        while(pushUpdateQueue.empty())              // wait for pushUpdateQueue to be non-empty
            pthread_cond_wait(&pushUpdateQueueCond, &pushUpdateQueueLock);      

        if(pushUpdateQueue.size() > 0)
        {
            action a = pushUpdateQueue.back();
            pushUpdateQueue.pop_back();        // pop the last element from the queue (critical section)
            out << "PU Thread " << index << " | ";
            out << "Push Update received" << " : " << a;
            pthread_mutex_unlock(&pushUpdateQueueLock);     // unlock the pushUpdateQueue
            for(int neighbor: graph[a.userID])
            {
                pthread_mutex_lock(&feedQueues[neighbor].lock);         // lock the feedQueue of the neighbor
                feedQueues[neighbor].push(a);                // push the action to the feedQueue of the neighbor (critical section)
                out << "PU Thread " << index << " | ";
                out << "Push Update sent to " << neighbor << " : " << feedQueues[neighbor].top();
                pthread_mutex_unlock(&feedQueues[neighbor].lock);           // unlock the feedQueue of the neighbor
                
                pthread_mutex_lock(&visLock);               // lock the visited set
                visited.insert(neighbor);                   // add the neighbor to the visited set (critical section)
                pthread_cond_broadcast(&visCond);               // broadcast the condition variable
                pthread_mutex_unlock(&visLock);             // unlock the visited set
            }
        }
        else pthread_mutex_unlock(&pushUpdateQueueLock);
    }
    return nullptr;
}