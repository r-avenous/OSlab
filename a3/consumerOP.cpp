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
#include <unordered_map>
#include <stack>

#define MAXNODES 5000
#define MAXDEGREE 2000
#define SETOFNODESSEGMENT 100
#define ADJACENCYLISTSEGMENT 200
#define PRODUCTIONSLEEP 50
#define CONSUMPTIONSLEEP 3

using namespace std;

int consumerID, totalNumNodes, shmid_setofnodes, shmid_adjlist, *setofnodes_segment, *adjlist_segment, oldTotalNumNodes;
bool optimisation;
char fileName[7];
vector<set<int>> adjList(MAXNODES);
set<int> allSources, newSources, oldSources;
unordered_map<int, vector<int>> shortestPathDistances, prevGlb;
int call = 1;
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
void printPathFromSourceToDest(int source, int dest, vector<int> &dist, vector<int> &prev, ofstream &fout, bool reversed = false)
{
    if(dist[dest] == 1e9) 
    {
        fout << source << " -> " << dest << " : No path\n";
        return;
    }
    if(reversed)
        fout << dest << " -> " << source << " : " << dist[dest] << " | ";
    else
        fout << source << " -> " << dest << " : " << dist[dest] << " | ";
    int u = dest;
    vector<int> path;
    while (u != source) 
    {
        path.push_back(u);
        u = prev[u];
    }
    path.push_back(source);
    if(!reversed)
        reverse(path.begin(), path.end());
    for (int i = 0; i < path.size(); i++) 
    {
        fout << path[i] << " ";
    }
    fout << endl;
}

void printPathFromSourceToAll(int source, vector<int>& dist, vector<int>& prev, ofstream &fout) 
{
    for(int i=0; i<totalNumNodes; i++)
    {
        if(source == i)
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
    vector<int> dist(MAXNODES, 1e9);
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
    prevGlb[source] = prev;
    shortestPathDistances[source] = dist;
    printPathFromSourceToAll(source, dist, prev, fout);
}

int main(int argc, char *argv[])
{
    consumerID = atoi(argv[1]);
    oldTotalNumNodes = 0;
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
        ofstream fout;
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
        if(!optimisation || call == 1) 
        {
            fout.open(fileName, ios::out);
            for(int src: allSources) singleSourceDijkstra(src, fout);
        }
        else
        {
            fout.open(fileName, ios::app);
            fout << endl;
            fout << "\n\n-----------------------------------call: " << call << "---------------------------------------\n\n";
            for(int i=oldTotalNumNodes; i<totalNumNodes; i++) 
            {
                // if(consumerID == 1) cout << i << endl;
                singleSourceDijkstra(i, fout);
                cout << i << endl;
                for(int oldsrc: oldSources)
                {
                    printPathFromSourceToDest(i, oldsrc, prevGlb[oldsrc], shortestPathDistances[oldsrc], fout, true);
                }
            }
            cout << "new to all done " << newSources.size() << endl;
            for(int oldsrc: oldSources)
            {
                for(int oldall = 0; oldall < oldTotalNumNodes; oldall++)
                {
                    if(oldsrc == oldall)
                        continue;
                    int closestNewSource = -1, minDist = shortestPathDistances[oldsrc][oldall];
                    for(int newall = oldTotalNumNodes; newall < totalNumNodes; newall++)
                    {
                        if(shortestPathDistances[newall][oldsrc] + shortestPathDistances[newall][oldall] < minDist)
                        {
                            minDist = shortestPathDistances[newall][oldsrc] + shortestPathDistances[newall][oldall];
                            closestNewSource = newall;
                        }
                    }
                    if(closestNewSource != -1)
                    {
                        shortestPathDistances[oldsrc][oldall] = minDist;
                        fout << "Updated: ";
                        printPathFromSourceToDest(closestNewSource, oldsrc, shortestPathDistances[closestNewSource], prevGlb[closestNewSource], fout, true);
                        printPathFromSourceToDest(closestNewSource, oldall, shortestPathDistances[closestNewSource], prevGlb[closestNewSource], fout, false);
                    }
                }
            }
        }
        // printPathInFile(fout);
        // cout << endl;
        fout.close();
        call++;
        newSources.clear();
        prevGlb.clear();
        cout << "Consumer " << consumerID << " is done consuming" << endl;
        oldTotalNumNodes = totalNumNodes;
        oldSources = allSources;
    }
    shmdt(adjlist_segment);      // deallocate shared memory segment; don't delete it yet!
    shmdt(setofnodes_segment);      // deallocate shared memory segment; don't delete it yet!
    return 0;
}