#include "userSimulator.hpp"
#include "helper.hpp"

extern int n;
extern unordered_map<int, vector<int>> graph;
extern vector<int> type;
extern Out out;
extern unordered_map<int, int> counter;

#define TIMEOUT 12

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

        for(int node: selectNodes)
        {
            action a(node, ++counter[node], rand()%3);
            out << a;
        }

        out << "User Simulator Sleeping.\n";
        sleep(TIMEOUT);
    }
    return nullptr;
}