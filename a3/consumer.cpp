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

#define MAXNODES 5000
#define MAXDEGREE 2000
#define SETOFNODESSEGMENT 100
#define ADJACENCYLISTSEGMENT 200
#define PRODUCTIONSLEEP 50
#define CONSUMPTIONSLEEP 30

using namespace std;

int consumerID, totalNumNodes;

inline int get_address_offset_setOfNodesSegment()
{
    return consumerID * (1+MAXNODES/10);
}

inline int get_address_offset_adjacencyListSegment(int node)
{
    return 1 + node * (1+MAXDEGREE);
}

int main(int argc, char *argv[])
{
    consumerID = atoi(argv[1]);
    cout << "Consumer " << consumerID << " is running" << endl;
    char fileName[7];
    sprintf(fileName, "%d.txt", consumerID);
    int shmid_setofnodes, shmid_adjlist, *setofnodes_segment, *adjlist_segment;
    shmid_setofnodes = shmget(SETOFNODESSEGMENT, 10 * (1 + MAXNODES/10) * sizeof(int), IPC_CREAT | 0666);          // create shared memory segment
    shmid_adjlist = shmget(ADJACENCYLISTSEGMENT, (1 + MAXNODES * (1 + MAXDEGREE)) * sizeof(int), IPC_CREAT | 0666);          // create shared memory segment

    setofnodes_segment = (int*)shmat(shmid_setofnodes, NULL, 0);                  // attach shared memory segment to process
    adjlist_segment = (int*)shmat(shmid_adjlist, NULL, 0);                  // attach shared memory segment to process

    while(1)
    {
        sleep(CONSUMPTIONSLEEP);
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
        vector<vector<int>> adjList(totalNumNodes);
        // get adjacency list
        for(int i=0; i<totalNumNodes; i++)
        {
            int* curAdjacencyList_segment = adjlist_segment + get_address_offset_adjacencyListSegment(i);
            int curNumNeighbors = *curAdjacencyList_segment;
            // cout << "The number of neighbors of node " << i << " is " << *curAdjacencyList_segment << endl;
            for(int j=1; j<=curNumNeighbors; j++)
            {
                // cout << *(curAdjacencyList_segment + j) << " ";
                adjList[i].push_back(*(curAdjacencyList_segment + j));
            }
            // cout << endl;
        }
        // cout << endl;
    }
    shmdt(adjlist_segment);      // deallocate shared memory segment; don't delete it yet!
    shmdt(setofnodes_segment);      // deallocate shared memory segment; don't delete it yet!
    return 0;
}