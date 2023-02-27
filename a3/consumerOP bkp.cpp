#include <iostream>
#include <cstring>
#include <vector>
#include <map>
#include <set>
#include <fstream>
#include <sys/shm.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <queue>
#include <algorithm>
#include <stack>

#define MAXNODES 5000
#define MAXDEGREE 2000
#define SETOFNODESSEGMENT 100
#define ADJACENCYLISTSEGMENT 200
#define PRODUCTIONSLEEP 50
#define CONSUMPTIONSLEEP 30

using namespace std;

int consumerID, totalNumNodes, shmid_setofnodes, shmid_adjlist, *setofnodes_segment, *adjlist_segment;
bool optimisation;
char fileName[7];
vector<set<int>> adjList(MAXNODES);
set<int> allSources, newSources;
// vector<vector<int>> paths(MAXNODES);

inline int get_address_offset_setOfNodesSegment()
{
    return consumerID * (1+MAXNODES/10);
}
inline int get_address_offset_adjacencyListSegment(int node)
{
    return 1 + node * (1+MAXDEGREE);
}
void signal_handler(int signum)
{
    shmdt(setofnodes_segment);
    shmdt(adjlist_segment);
    exit(0);
}

// void printPathInFile(ofstream &fout)
// {
//     for(int i=0; i<totalNumNodes; i++)
//     {
//         if(allSources.find(i) != allSources.end())
//             continue;
//         else if(paths[i].size() == 0)
//             fout << i << " : No path\n";
//         else
//         {
//             fout << i << " <- " << paths[i][0] << " : " << paths[i].size() - 1 << " | ";
//             for (int j = 0; j < paths[i].size(); j++) 
//             {
//                 fout << paths[i][j] << " ";
//             }
//             fout << endl;
//         }
//     }
// }

void printPathFromSourceToAll(int source, vector<int>& dist, vector<int>& prev, ofstream &fout) 
{
    for(int i=0; i<totalNumNodes; i++)
    {
        if(allSources.find(i) != allSources.end())
            continue;
        else if(dist[i] == 1e9)
            fout << source << " -> " << i << " : No path\n";
        else
        {
            fout << source << " -> " << i << " : " << dist[i] << " | ";
            int u = i;
            stack<int> path;
            while (u != source) 
            {
                path.push(u);
                u = prev[u];
            }
            path.push(source);
            while (!path.empty()) 
            {
                fout << path.top() << " ";
                path.pop();
            }
            fout << endl;
        }
    }
}

void singleSourceDijkstra(int source, ofstream &fout) 
{
    queue<int> q;
    vector<bool> visited(totalNumNodes, false);
    vector<int> dist(totalNumNodes, 1e9);
    vector<int> prev(totalNumNodes, -1);
    dist[source] = 0;
    visited[source] = true;
    q.push(source);
    while (!q.empty()) 
    {
        int u = q.front();
        q.pop();
        for (int v : adjList[u]) 
        {
            if (!visited[v]) 
            {
                visited[v] = true;
                dist[v] = dist[u] + 1;
                prev[v] = u; // store predecessor
                q.push(v);
            }
        }
    }
    printPathFromSourceToAll(source, dist, prev, fout);
}


int main(int argc, char *argv[])
{
    consumerID = atoi(argv[1]);
    sprintf(fileName, "%d.txt", consumerID);
    cout << "Consumer " << consumerID << " is running";
    signal(SIGINT, signal_handler);
    optimisation = false;
    if(argc > 2)
    {
        if(strcmp(argv[2], "-optimize") == 0)
        {
            optimisation = true;
            cout << " optimised";
        }
    }
    cout << endl;
    
    shmid_setofnodes = shmget(SETOFNODESSEGMENT, 10 * (1 + MAXNODES/10) * sizeof(int), IPC_CREAT | 0666);          // create shared memory segment
    shmid_adjlist = shmget(ADJACENCYLISTSEGMENT, (1 + MAXNODES * (1 + MAXDEGREE)) * sizeof(int), IPC_CREAT | 0666);          // create shared memory segment

    setofnodes_segment = (int*)shmat(shmid_setofnodes, NULL, 0);                  // attach shared memory segment to process
    adjlist_segment = (int*)shmat(shmid_adjlist, NULL, 0);                  // attach shared memory segment to process

    while(1)
    {
        sleep(CONSUMPTIONSLEEP);
        ofstream file;
        int oldSourceNum = allSources.size();
        // get set of nodes
        int* thisProcessSetOfNodes_segment = setofnodes_segment + get_address_offset_setOfNodesSegment();
        int thisProcessNumNodes = *thisProcessSetOfNodes_segment;
        // cout << "The number of nodes is " << *curAdjacencyList_segment << endl;
        cout << "\nConsumer " << consumerID << " is consuming " << thisProcessNumNodes << " nodes:" << endl;
        for(int i=oldSourceNum+1; i<=thisProcessNumNodes; i++)
        {
            // cout << *(thisProcessSetOfNodes_segment + i) << " ";
            newSources.insert(*(thisProcessSetOfNodes_segment + i));
            allSources.insert(*(thisProcessSetOfNodes_segment + i));
        }
        totalNumNodes = *adjlist_segment;
        // get adjacency list
        for(int i=0; i<totalNumNodes; i++)
        {
            int* curAdjacencyList_segment = adjlist_segment + get_address_offset_adjacencyListSegment(i);
            int curNumNeighbors = *curAdjacencyList_segment, oldlistsize = adjList[i].size();
            // cout << "The number of neighbors of node " << i << " is " << *curAdjacencyList_segment << endl;
            for(int j=1+oldlistsize; j<=curNumNeighbors; j++)
            {
                // cout << *(curAdjacencyList_segment + j) << " ";
                adjList[i].insert(*(curAdjacencyList_segment + j));
            }
            // cout << endl;
        }
        if(!optimisation) 
        {
            file.open(fileName, ios::out);
            for(int src: allSources) singleSourceDijkstra(src, file);
        }
        else
        {
            file.open(fileName, ios::app);
            file << endl;
            for(int src : newSources) singleSourceDijkstra(src, file);
        }
        // printPathInFile(file);
        // cout << endl;
        file.close();
        newSources.clear();
        cout << "Consumer " << consumerID << " is done consuming" << endl;
    }
    shmdt(adjlist_segment);      // deallocate shared memory segment; don't delete it yet!
    shmdt(setofnodes_segment);      // deallocate shared memory segment; don't delete it yet!
    return 0;
}