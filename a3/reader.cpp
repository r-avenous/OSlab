#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <sys/shm.h>
#define NUM_EDGES 88234

using namespace std;

int main(){

    char *shared_seg;
    key_t key;
    int shmid;
    int memory_size = NUM_EDGES * sizeof(int) * 2 + sizeof(int); // allocate memory for graph edges, as well as number of edges

    key = ftok("/tmp", 'a');                                     // generate unique key for shmget()
    shmid = shmget(key, memory_size, IPC_CREAT | 0666);          // create shared memory segment
    shared_seg = (char *)shmat(shmid, NULL, 0);                  // attach shared memory segment to process

    // define two array pointers to point to starting address of the 1st and 2nd node arrays
    // read number of edges from shared memory
    // because this info cannot be passed by main process to other processes otherwise
    int num_edges = *((int *)shared_seg);           
    int *first_node_arr = (int *)shared_seg + 1;
    int *second_node_arr = (int *)shared_seg + (num_edges + 1);
    
    // read graph edges from shared memory
    int *first_node_ptr = first_node_arr;
    int *second_node_ptr = second_node_arr;

    // verify the graph edges stored in shared memory
    printf("Number of edges: %d\n", num_edges);
    printf("Edges in the graph: \n\n");

    for(int i = 0; i < num_edges; i++)
    {
        cout << *first_node_ptr++ << " " << *second_node_ptr++ << endl;
    }

    shmdt(shared_seg);                  // detach shared memory segment
    shmctl(shmid, IPC_RMID, NULL);      // delete shared memory segment
    return 0;
}