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
#include <bits/stdc++.h>
#include <signal.h>

using namespace std;

#define MAXNODES 5000
#define MAXDEGREE 2000
#define PRODUCTIONSLEEP 50
#define CONSUMPTIONSLEEP 30
#define SETOFNODESSEGMENT 100
#define ADJACENCYLISTSEGMENT 200

int *adj_list, *mapping;

void sig_handler(int sig_no){ // if SIGINT is detected, first detach the shared memeory segment and then exit
    shmdt(adj_list);
    shmdt(mapping);
    exit(0);
}

int main(){
    signal(SIGINT, sig_handler); // handler for SIGINT signal
    int shmid_adj_list, shmid_mapping;
    int i=0, j=0;
    shmid_adj_list = shmget(ADJACENCYLISTSEGMENT, (MAXNODES * (1 + MAXDEGREE) + 1) * sizeof(int), IPC_CREAT | 0666); // gets the memory segment with provided key (for adj_list)
    shmid_mapping = shmget(SETOFNODESSEGMENT, (10 * (1 + MAXNODES/10) ) * sizeof(int) , IPC_CREAT | 0666); // gets the memory segment with provided key (for mapping)
    adj_list = (int *)shmat(shmid_adj_list, NULL, 0); //attach adj_list
    mapping = (int *)shmat(shmid_mapping, NULL, 0); // attach_mapping
    // int countr = 1;
    srand(time(0));
    while(1)
    {
        int m, k ,ubm = 5, lbm = 1, ubk = 3, lbk = 1;
        // unsigned long int sum_deg = 0;
        // srand(time(0));
        // m = (rand() % (ubm - lbm + 1)) + lbm; // generate value of m
        m=1;
        // cout << "m: " << m << endl;
        int num_nodes = adj_list[0]; // num_nodes = total_number of nodes
            cout << "num_nodes: " << num_nodes << endl;
        vector<int> degree;          // vector to store degree of all nodes in a prefix sum fashion
        // int temp;
        unsigned long long sum=0;      // summ will store the sum of all degrees
        for(i=0; i<num_nodes; i++){
            int temp = adj_list[1+i*500];
            // cout << "temp: " << temp << endl;
            sum += temp;
            if(i!= 0){
                temp += *(degree.end()-1);
                degree.push_back(temp);
            }
            else{
                degree.push_back(temp);
            }
        }
        // cout << "sum: " << sum << "size " << degree.size() << endl;

        // the method being used for finding the new k nodes:
        /*
            We first sum of all degrees and pick a random number from (0, sum of all degrees - 1).
            After this we find in which interval does it lie. For example:
            if degrees of node are d1, d2, d3.....
            and if the value lies between d1, d1+d2 then we add the edge between new node and the second node.
        */
        
        for(i=0;i<m;i++){
            k = (rand() % (ubk - lbk + 1)) + lbk;
            // cout << "k: " << k << endl;
            unordered_map<int, int> ignore;          // ignore will consist keys of those nodes which are already added as an edge
            for(j=0; j<k; j++){
                int random = (int)((double)rand() / ((double)RAND_MAX + 1) * sum); // generate random value
                sleep(1);
                auto upper = upper_bound(degree.begin(), degree.end(), random); // use binary search to find its interval and get the corresponding node
                int new_node = upper - degree.begin(); // new_node  is the node which is to get added as an edge
                if(ignore.find(new_node) == ignore.end()){ // we check if it has already been selected
                    ignore.insert({new_node,1});
                }
                else{
                    j--;
                }
                // cout << random << " " << new_node << endl;
            }
            int ptr = 1 + (MAXDEGREE+1)*(num_nodes+i);  // for the newly added node first we add the degree in the adj_list
            adj_list[ptr] = ignore.size();
            int l=1;
            // cout << "ignore size: " << ignore.size() << endl;
            for(auto it = ignore.begin(); it != ignore.end(); it++){ // now we iterate over all the vertices who got added as a new edge to update their adj_list
                adj_list[ptr + l] = it->first;
                l++;
                int temp_ptr = 1 + (it->first)*(MAXDEGREE+1);
                adj_list[temp_ptr] += 1; // first we increase the degree by 1
                int temp_node = adj_list[temp_ptr];
                adj_list[temp_ptr + temp_node] = num_nodes + i; // then we add the new node's node number
                // cout << "\ntemp_ptr: " << temp_ptr << " temp_node: " << temp_node << " adj " << adj_list[temp_ptr + temp_node] << "it -> first " << it->first << endl;
                // for(int i=0; i<adj_list[temp_ptr]; i++){
                //     cout << " " << adj_list[temp_ptr + 1 + i];
                // }
            }
            int rand_consumer;
            rand_consumer = rand()%10; // randomly selecting a consumer out of the 10 partitions
            // cout << "rand_consumer: " << rand_consumer << endl;
            int temp_ptr = rand_consumer*(1 + MAXNODES/10); 
            mapping[temp_ptr] += 1; // increasing the number of nodes in the partition of randomly selected consumer
            mapping[temp_ptr + mapping[temp_ptr]] = num_nodes + i; // adding the new node into the partition
        }
        adj_list[0] += m; // increase the total number count
        // sleep(50); // producer sleeps for 50s after each iteration
    }
    
}
