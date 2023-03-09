#include "userSimulator.hpp"
#include "helper.hpp"
#include <math.h>

extern int n;
extern unordered_map<int, vector<int>> graph;
extern Out out;
extern unordered_map<int, int> counter;
extern unordered_map<int, vector<action>> wallQueue;

vector<action> pushUpdateQueue;
pthread_mutex_t pushUpdateQueueLock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t pushUpdateQueueCond = PTHREAD_COND_INITIALIZER;

#define TIMEOUT 12
#define PROPORTIONALITY 1

void *userSimulator(void *arg)
{
    while(true)
    {
        out << "User simulator awake\n";
        // select 100 random nodes
        set<int> selectNodes;
        while((int)selectNodes.size() < 100) selectNodes.insert(rand() % n + 1);

        out << "Selected Nodes: ";
        for(int i: selectNodes) out << i << ' ';
        out << '\n';

        unordered_map<int, vector<action>> xp;

        for(int node: selectNodes)
        {
            int numActions = PROPORTIONALITY * log2(graph[node].size()) + PROPORTIONALITY;
            out << "User " << node << " will perform " << numActions << " actions." << " | Degree: " << graph[node].size() << '\n';
            for(int j=0; j<numActions; j++)
            {
                action a(node, ++counter[node], rand()%3);
                // out << a;
                xp[j].push_back(a);
                wallQueue[node].push_back(a);
            }
        }
        for(auto p: xp)
        {
            for(action a: p.second)
            {
                pthread_mutex_lock(&pushUpdateQueueLock);
                pushUpdateQueue.push_back(a);
                out << a;
                pthread_cond_broadcast(&pushUpdateQueueCond);
                pthread_mutex_unlock(&pushUpdateQueueLock);
            }
        }

        out << "User Simulator Sleeping.\n";
        sleep(TIMEOUT);
    }
    return nullptr;
}