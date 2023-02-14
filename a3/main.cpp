#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <sys/shm.h>
#define NUM_EDGES 88234

using namespace std;

int main(){

    const char *filename = "facebook_combined.txt";
    char *shared_seg;
    key_t key;
    int shmid;
    int memory_size = NUM_EDGES * sizeof(int) * 2 + sizeof(int); // allocate memory for graph edges, as well as number of edges

    key = ftok("/tmp", 'a');                                     // generate unique key for shmget()
    shmid = shmget(key, memory_size, IPC_CREAT | 0666);          // create shared memory segment
    shared_seg = (char *)shmat(shmid, NULL, 0);                  // attach shared memory segment to process

    // define two arrays to store 1st and 2nd nodes of graph edges; store number of edges in the last int block
    int *num_edges = (int *)shared_seg;
    int *first_node_arr = (int *)shared_seg + 1;
    int *second_node_arr = (int *)shared_seg + (NUM_EDGES + 1);

    // read graph edges from file
    ifstream file(filename);
    string line;

    // assign pointers to the first element of the arrays
    *num_edges = NUM_EDGES;
    int *first_node_ptr = first_node_arr;
    int *second_node_ptr = second_node_arr;

    // read graph edges from file and write them to shared memory
    while(getline(file, line))
    {
        int node1, node2;
        sscanf(line.c_str(), "%d %d", &node1, &node2);
        *first_node_ptr++ = node1;
        *second_node_ptr++ = node2;
    }

    first_node_ptr = first_node_arr;
    second_node_ptr = second_node_arr;

    // display graph edges from shared memory for verification
    printf("Number of edges: %d\n", *num_edges);
    printf("Edges in the graph: \n\n");
    
    for(int i = 0; i < *num_edges; i++)
    {
        cout << *first_node_ptr++ << " " << *second_node_ptr++ << endl;
    }

    shmdt(shared_seg);      // deallocate shared memory segment; don't delete it yet!
    return 0;
}