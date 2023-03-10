#include "userSimulator.hpp"
#include "helper.hpp"
#include <math.h>
#include <unistd.h>

extern int n;
extern unordered_map<int, vector<int>> graph;
extern Out out;
extern unordered_map<int, int> counter[3];
extern unordered_map<int, vector<action>> wallQueue;

vector<action> pushUpdateQueue;         // pushUpdateQueue
pthread_mutex_t pushUpdateQueueLock = PTHREAD_MUTEX_INITIALIZER;        // lock for pushUpdateQueue
pthread_cond_t pushUpdateQueueCond = PTHREAD_COND_INITIALIZER;          // condition variable for pushUpdateQueue

#define TIMEOUT 20
#define PROPORTIONALITY 1

void *userSimulator(void *arg)
{
    srand(time(NULL) + *(int*)arg);
    while(true)
    {
        out << "\n\n\nUser simulator awake\n";
        // select 100 random nodes
        set<int> selectNodes;
        while((int)selectNodes.size() < 1) selectNodes.insert((int)(((double)rand()/((double)RAND_MAX +1)) * n));

        out << "Selected Nodes: ";
        for(int i: selectNodes) out << i << ' ';
        out << '\n';

        // unordered_map<int, vector<action>> xp;

        for(int node: selectNodes)
        {
            int numActions = PROPORTIONALITY * log2(graph[node].size()) + PROPORTIONALITY;
            out << "User " << node << " will perform " << numActions << " actions." << " | Degree: " << graph[node].size() << '\n';
            for(int j=0; j<numActions; j++)
            {
                srand(time(NULL) + j + node + *(int*)arg);
                int type = rand() % 3;
                action a(node, ++counter[type][node], type);
                // out << a;
                pthread_mutex_lock(&pushUpdateQueueLock);       // lock pushUpdateQueue
                pushUpdateQueue.push_back(a);            // push action to pushUpdateQueue (critical section)
                out << a;
                pthread_cond_broadcast(&pushUpdateQueueCond);       // broadcast to all threads waiting on pushUpdateQueueCond
                pthread_mutex_unlock(&pushUpdateQueueLock);     // unlock pushUpdateQueue
                // xp[j].push_back(a);
                wallQueue[node].push_back(a);       // push action to wallQueue
            }
        }
        // for(auto p: xp)
        // {
        //     for(action a: p.second)
        //     {
                
        //     }
        // }

        out << "User Simulator Sleeping.\n";
        sleep(TIMEOUT);
    }
    return nullptr;
}