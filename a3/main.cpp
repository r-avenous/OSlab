#include <iostream>
#include <cstring>
#include <vector>
#include <set>
#include <fstream>
#include <sys/shm.h>
#define NUM_NODES 4039
#define NUM_EDGES 88234
#define MAX_NODES 1e4
#define MAX_EDGES 1e6

using namespace std;

int main(){

    const char *filename = "facebook_combined.txt";
    vector<set<int>> adjList(NUM_NODES);
    int node1, node2;

    // read graph edges from file
    ifstream file(filename);
    string line;
    
    while (getline(file, line)){

        sscanf(line.c_str(), "%d %d", &node1, &node2);
        adjList[node1].insert(node2);
        adjList[node2].insert(node1);
    }

    char *shared_seg;
    key_t key;
    int shmid;
    int memory_size = (1 + MAX_NODES + 2 * MAX_EDGES) * sizeof(int); // allocate memory for graph edges, as well as number of edges

    key = ftok("/tmp", 'a');                                     // generate unique key for shmget()
    shmid = shmget(key, memory_size, IPC_CREAT | 0666);          // create shared memory segment
    shared_seg = (char *)shmat(shmid, NULL, 0);                  // attach shared memory segment to process

    int *num_nodes = (int *)shared_seg;
    int *nbrs_cnt_ptr = num_nodes + 1;
    int *nbrs_ptr = num_nodes + 2;
    set<int> nbrs;
    *num_nodes = NUM_NODES;

    // load graph into shared memory 
    for (int i = 0; i < *num_nodes; i++){

        *nbrs_cnt_ptr = adjList[i].size();
        for (auto it = adjList[i].begin(); it != adjList[i].end(); it++){

            *nbrs_ptr = *it;
            nbrs_ptr++;
        }

        nbrs_cnt_ptr = nbrs_ptr;
        nbrs_ptr++;
    }

    num_nodes = (int *)shared_seg;
    nbrs_cnt_ptr = num_nodes + 1;
    nbrs_ptr = num_nodes + 2;

    // for testing 
    for (int i = 0; i < *num_nodes; i++){

        cout << "No. of neighbors of " << i << " : " << *nbrs_cnt_ptr << endl;
        cout << i << " : ";
        for (auto it = adjList[i].begin(); it != adjList[i].end(); it++){

            cout << *nbrs_ptr << " ";
            nbrs_ptr++;
        }

        cout << endl;
        nbrs_cnt_ptr = nbrs_ptr;
        nbrs_ptr++;
    }

    shmdt(shared_seg);      // deallocate shared memory segment; don't delete it yet!
    shmctl(shmid, IPC_RMID,NULL);
    return 0;
}