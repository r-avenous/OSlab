#include <iostream>
#include <cstring>
#include <vector>
#include <fstream>
#include <sys/shm.h>

using namespace std;

class Graph {

    int V;
    int E;
    vector<int>* adj;

    public:
        Graph(int V, int E) 
        {
            this->V = V;
            this->E = E;
            adj = new vector<int>[V];
        }

        void addEdge(int v, int w) 
        {
            adj[v].push_back(w);
            adj[w].push_back(v);
        }

        size_t calculateGraphSize(){

            int size = 0;
            for (int v = 0; v < V; v++) 
                size += adj[v].size();
            
            cout << "Graph size in bytes: " << size * sizeof(int) << endl;
            return size * sizeof(int);
        }

        void printGraph() 
        {
            for (int v = 0; v < V; v++) 
            {
                cout << "\n Adjacency list of vertex " << v << "\n head ";
                for (auto x : adj[v])
                    cout << "-> " << x;
                printf("\n");
            }
        }
};

int main(){

    const char *filename = "facebook_combined.txt";
    Graph graph(4039, 88234);

    // Read the file
    ifstream file(filename);
    string line;

    while(getline(file, line))
    {
        int node1, node2;
        sscanf(line.c_str(), "%d %d", &node1, &node2);
        graph.addEdge(node1, node2); 
    }

    char *myseg;
    key_t key;
    int shmid;

    shmid = shmget(key, graph.calculateGraphSize(), IPC_CREAT | 0666);
    myseg = (char *)shmat(shmid, NULL, 0);

    shmdt(myseg);
    shmctl(shmid, IPC_RMID, NULL);
    return 0;
}