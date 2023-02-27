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
#define CONSUMPTIONSLEEP 30

using namespace std;

int consumerID, totalNumNodes, shmid_setofnodes, shmid_adjlist, *setofnodes_segment, *adjlist_segment, oldTotalNumNodes;
bool optimisation;
char fileName[7];
vector<set<int>> adjList(MAXNODES); // set of all adjacency lists
set<int> allSources, newSources, oldSources;    // stores all , new and old sources respectively
unordered_map<int, vector<int>> shortestPathDistances, prevGlb; //maps to store shortest path vector and parent vector of each node for each source node
int call = 1;
// vector<vector<int>> paths(MAXNODES);

inline int get_address_offset_setOfNodesSegment()   // inline function to find the offset of the current consumer
{
    return consumerID * (1+MAXNODES/10);
}
inline int get_address_offset_adjacencyListSegment(int node) // inline function to find offset of adjacency list of node 'node'
{
    return 1 + node * (1+MAXDEGREE);
}
void signal_handler(int signum) // signal handler for SIGINT (detaches the memory segments before exiting)
{
    shmdt(setofnodes_segment);
    shmdt(adjlist_segment);
    exit(0);
}

// function to print path from source to dest
vector<int> printPathFromSourceToDest(int source, int dest, ofstream &fout, bool reversed = false, bool print = true)
{
    // if there doesnt exist any shortest path from source to dest
    if(shortestPathDistances[source][dest] == 1e9) 
    {
        if(print) fout << source << " -> " << dest << " : No path\n";
        return {};
    }
    // if path needs to be printed in reverse
    if(reversed){
        if(print) fout << dest << " -> " << source << " : " << shortestPathDistances[source][dest] << " | ";
    }
    else{
        if(print) fout << source << " -> " << dest << " : " << shortestPathDistances[source][dest] << " | ";
    }
    int u = dest;
    vector<int> path;
    // store the path in vector named path (it stores in reverse order)
    while (u != source) 
    {
        path.push_back(u);
        u = prevGlb[source][u];
    }
    path.push_back(source);
    // if we dont want reverse path then we reverse the vector path to get back original path
    if(!reversed)
        reverse(path.begin(), path.end());
    // printing the path
    for (int i = 0; i < (int)path.size(); i++) 
    {
        if(print) fout << path[i] << " ";
    }
    if(print) fout << endl;
    return path;
}

// function to print path from source to all nodes
void printPathFromSourceToAll(int source, vector<int>& dist, vector<int>& prev, ofstream &fout) 
{
    for(int i=0; i<totalNumNodes; i++)
    {
        // if source itself then continue
        if(i == source)
            continue;
        // if no path exists from source to this new node
        else if(dist[i] == 1e9)
            fout << source << " -> " << i << " : No path\n";
        // print path from the source to this node
        else
        {
            fout << source << " -> " << i << " : " << dist[i] << " | ";
            int u = i;
            stack<int> path;
            // store the path in stack named path
            while (u != source) 
            {
                path.push(u);
                u = prev[u];
            }
            path.push(source);
            // printing the path (in reverse)
            while (!path.empty()) 
            {
                fout << path.top() << " ";
                path.pop();
            }
            fout << endl;
        }
    }
}

// function to run single source dijkstra
void singleSourceDijkstra(int source, ofstream &fout) 
{
    queue<int> q;
    vector<bool> visited(totalNumNodes, false); // vector to store visited nodes
    vector<int> dist(MAXNODES, 1e9);            // vector to store the distances intialized with 1e9
    vector<int> prev(totalNumNodes, -1);        // vector to store the predecessor of each node
    dist[source] = 0;                           // intialize source distance as 0
    visited[source] = true;                     // make visited to source true
    q.push(source);
    // while queue is not empty keep finding the nearest node and check if visited or not and add
    while (!q.empty())                          
    {
        int u = q.front();
        q.pop();
        for (int v : adjList[u]) 
        {
            // if not visited
            if (!visited[v]) 
            {
                visited[v] = true; // make visited true for this node
                dist[v] = dist[u] + 1; // update the distance
                prev[v] = u; // store predecessor
                q.push(v); // push the node into the queue
            }
        }
    }
    // update parent values vector (parents of all nodes) for the source node (used in printing)
    prevGlb[source] = prev; 
    // update the shortest distance vector (shortest distance of all the nodes from the source node) for the source node
    shortestPathDistances[source] = dist;
    // print the path from source to all nodes
    
    if(allSources.find(source) != allSources.end())
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
    // check if optimization flag is passed 
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
        // if optimisation is false or it is the first call then run normal single source dijkstra on all nodes without optimization
        if(!optimisation || call == 1) 
        {
            fout.open(fileName, ios::out);
            fout << "\n\n-----------------------------------call: " << call << "---------------------------------------\n\n";
            for(int src: allSources) singleSourceDijkstra(src, fout);
        }
        // if optimization is to be done
        else
        {
            fout.open(fileName, ios::app);
            fout << "\n\n-----------------------------------call: " << call << "---------------------------------------\n\n";
            for(int i=oldTotalNumNodes; i<totalNumNodes; i++) 
            {
                // if(consumerID == 1) cout << i << endl;
                // run single source dijkstra on all new nodes and also print paths from the new node
                singleSourceDijkstra(i, fout);
                // cout << i << endl;
                // print path from old source to new by reversing the path from new to old which is already found before
                for(int oldsrc: oldSources)
                {
                    // cout << shortestPathDistances[i][oldsrc] << " " << prevGlb[i].size() << endl;
                    // call function to print path from old sources to new nodes
                    printPathFromSourceToDest(i, oldsrc, fout, true);
                    shortestPathDistances[oldsrc][i] = shortestPathDistances[i][oldsrc];
                }
            }
            cout << "New nodes to all others done." << endl;
            // find all pairs of old nodes and select a new node and check if the distance between the old nodes needs to be updated based on the optimization
            for(int oldsrc: oldSources)
            {
                for(int oldall = 0; oldall < oldTotalNumNodes; oldall++)
                {
                    if(oldsrc == oldall)
                        continue;
                    int closestNewSource = -1, minDist = shortestPathDistances[oldsrc][oldall];
                    for(int newall = oldTotalNumNodes; newall < totalNumNodes; newall++)
                    {
                        // if distance between the old nodes is less than distance from old1 to new node and old2 to new node then update flag value of closesNewSource
                        if(shortestPathDistances[newall][oldsrc] + shortestPathDistances[newall][oldall] < minDist)
                        {
                            minDist = shortestPathDistances[newall][oldsrc] + shortestPathDistances[newall][oldall];
                            closestNewSource = newall;
                        }
                    }
                    // if flag value of closestNewSource is not -1 means that we found a new node which updates the distance between the old nodes
                    if(closestNewSource != -1)
                    {
                        fout << "Updated from old distance " << shortestPathDistances[oldsrc][oldall] << " to " 
                            << minDist << " by " << closestNewSource << " as new intermediate" << endl;
                        shortestPathDistances[oldsrc][oldall] = minDist; // update the shortest path distance between the old nodes
                        vector<int> p1 = printPathFromSourceToDest(closestNewSource, oldsrc, fout, true, false); // p1 holds the path from oldsrc to new node
                        vector<int> p2 = printPathFromSourceToDest(closestNewSource, oldall, fout, false, false); // p2 holds the path from new node to oldall
                        fout << oldsrc << " -> " << oldall << " : " << minDist << " | ";
                        // print the paths
                        for(int i=0; i<(int)p1.size(); i++) fout << p1[i] << " ";
                        for(int i=1; i<(int)p2.size(); i++) fout << p2[i] << " ";
                        fout << endl;
                    }
                }
            }
        }
        fout.close();
        call++;
        newSources.clear();
        prevGlb.clear();
        cout << "Consumer " << consumerID << " is done consuming" << endl;
        oldTotalNumNodes = totalNumNodes;
        oldSources = allSources;
    }
    shmdt(adjlist_segment);      // detach shared memory segment;
    shmdt(setofnodes_segment);      // detach shared memory segment;
    return 0;
}