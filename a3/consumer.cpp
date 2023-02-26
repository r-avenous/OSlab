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

#define MAXNODES 5000
#define MAXDEGREE 2000
#define SETOFNODESSEGMENT 100
#define ADJACENCYLISTSEGMENT 200
#define PRODUCTIONSLEEP 50
#define CONSUMPTIONSLEEP 30

using namespace std;

int consumerID, totalNumNodes, shmid_setofnodes, shmid_adjlist, *setofnodes_segment, *adjlist_segment;
char fileName[7];
vector<set<int>> adjList(MAXNODES);

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

void printDestinationPaths(const vector<int>& sources, vector<int>& dests, vector<vector<int>>& dist, ofstream &fout) 
{
    set<int> sourcesSet(sources.begin(), sources.end());
    for (int i = 0; i < (int)dests.size(); i++) 
    {
        vector<int> path;
        int closestSrc = -1;
        int minDist = 1e9;
        for (int j = 0; j < (int)sources.size(); j++) 
        {
            if (dist[j][dests[i]] < minDist) 
            {
                closestSrc = sources[j];
                minDist = dist[j][dests[i]];
            }
        }
        fout << dests[i] << " <- ";
        if (minDist == 1e9) 
        {
            fout << "X : No path.\n";
            continue;
        }
        int u = dests[i];
        path.push_back(u);
        while (sourcesSet.find(u) == sourcesSet.end())
        {
            for (int v : adjList[u]) 
            {
                if (dist[sources.size() - 1][v] == dist[sources.size() - 1][u] - 1) 
                {
                    u = v;
                    break;
                }
            }
            path.push_back(u);
        }
        fout << path[path.size() - 1] << " : " << minDist << " | ";
        for (int j = (int)path.size() - 1; j >= 0; j--) 
        {
            fout << path[j] << " ";
        }
        fout << endl;        
    }
}

void multiSourceDijkstra(const vector<int>& sources, ofstream &fout) 
{
    set<int> sourcesSet(sources.begin(), sources.end());
    queue<int> q;
    vector<bool> visited(totalNumNodes, false);
    vector<vector<int>> dist(sources.size(), vector<int>(totalNumNodes, 1e9));
    for (int i = 0; i < (int)sources.size(); i++) 
    {
        dist[i][sources[i]] = 0;
        visited[sources[i]] = true;
        q.push(sources[i]);
    }
    while (!q.empty()) 
    {
        int u = q.front();
        q.pop();
        for (int v : adjList[u]) 
        {
            if (!visited[v]) {
                visited[v] = true;
                for (int i = 0; i < (int)sources.size(); i++) {
                    dist[i][v] = dist[i][u] + 1;
                }
                q.push(v);
            }
        }
    }
    vector<int> dests;
    for (int i = 0; i < totalNumNodes; i++) 
    {
        if(sourcesSet.find(i) == sourcesSet.end())
            dests.push_back(i);
    }
    printDestinationPaths(sources, dests, dist, fout);
}

int main(int argc, char *argv[])
{
    consumerID = atoi(argv[1]);
    sprintf(fileName, "%d.txt", consumerID);
    cout << "Consumer " << consumerID << " is running" << endl;
    
    shmid_setofnodes = shmget(SETOFNODESSEGMENT, 10 * (1 + MAXNODES/10) * sizeof(int), IPC_CREAT | 0666);          // create shared memory segment
    shmid_adjlist = shmget(ADJACENCYLISTSEGMENT, (1 + MAXNODES * (1 + MAXDEGREE)) * sizeof(int), IPC_CREAT | 0666);          // create shared memory segment

    setofnodes_segment = (int*)shmat(shmid_setofnodes, NULL, 0);                  // attach shared memory segment to process
    adjlist_segment = (int*)shmat(shmid_adjlist, NULL, 0);                  // attach shared memory segment to process

    while(1)
    {
        sleep(CONSUMPTIONSLEEP);
        ofstream file(fileName);
        set<int> setOfNodes;
        // get set of nodes
        int* thisProcessSetOfNodes_segment = setofnodes_segment + get_address_offset_setOfNodesSegment();
        int thisProcessNumNodes = *thisProcessSetOfNodes_segment;
        // cout << "The number of nodes is " << *curAdjacencyList_segment << endl;
        cout << "\nConsumer " << consumerID << " is consuming " << thisProcessNumNodes << " nodes:" << endl;
        for(int i=1; i<=thisProcessNumNodes; i++)
        {
            // cout << *(thisProcessSetOfNodes_segment + i) << " ";
            setOfNodes.insert(*(thisProcessSetOfNodes_segment + i));
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
        multiSourceDijkstra(vector<int> (setOfNodes.begin(), setOfNodes.end()), file);
        // cout << endl;
        file.close();
        cout << "Consumer " << consumerID << " is done consuming" << endl;
    }
    shmdt(adjlist_segment);      // deallocate shared memory segment; don't delete it yet!
    shmdt(setofnodes_segment);      // deallocate shared memory segment; don't delete it yet!
    return 0;
}