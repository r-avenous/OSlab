#include "helper.hpp"
#include "userSimulator.hpp"
#include "readPost.hpp"
#include "pushUpdate.hpp"

int n;          // number of nodes
unordered_map<int, vector<int>> graph;          // graph as adjacency list
Out out;        // output stream using custom class Out
unordered_map<int, int> counter[3];         // counter for each type of action for each node
unordered_map<int, vector<action>> wallQueue;       // wall queues for each node
unordered_map<int, feedQueue> feedQueues;           // feed queues for each node
void sig_handler(int signo) // signal handler; not required here but good to have
{
    if (signo == SIGINT)
    {
        cout << "SIGINT received\n";
        exit(0);
    }
}

int main()
{
    signal(SIGINT, sig_handler);
    // read csv file and store in array
    ifstream file(GRAPHFILE);
    string value;
    getline(file, value); // skip first line (header)
    set<int> nodes;
    while (file.good())
    {
        getline(file, value);
        if(value.length() == 0) break; // skip last line (empty)
        int pos = value.find(",");
        int a = stoi(value.substr(0, pos)), b = stoi(value.substr(pos+1, value.length()));
        graph[a].push_back(b);
        graph[b].push_back(a);
        nodes.insert(a);
        nodes.insert(b);
    }
    file.close();           // close file
    n = nodes.size();       // number of nodes
    nodes.clear();      // clear set
    for(int i=0; i<n; i++)
    {
        // assign random type 0 or 1
        int type = rand() % 2;      // type 0 or 1
        feedQueues[i] = feedQueue(type, i);     // create feed queue for each node
    }
    pthread_t userSimulatorThread, readPostThread[10], pushUpdateThread[25];        // threads  for userSimulator, readPost and pushUpdate
    pthread_create(&userSimulatorThread, NULL, userSimulator, NULL);    // create userSimulator thread
    cout << "Creating pushUpdate" << endl;
    for(int i=0; i<25; i++)
    {       // create 25 pushUpdate threads
        int *index = new int(i+1);
        pthread_create(&pushUpdateThread[i], NULL, pushUpdate, (void*)index);
    }
    cout << "Creating readPost" << endl;
    for(int i=0; i<10; i++)
    {       // create 10 readPost threads
        int *index = new int(i+1);
        pthread_create(&readPostThread[i], NULL, readPost, (void*)index);
    }
    cout << "Executing readPost" << endl;
    for(int i=0; i<10; i++)
    {       // join 10 readPost threads
        pthread_join(readPostThread[i], NULL);
    }
    cout << "Executing pushUpdate" << endl;
    for(int i=0; i<25; i++)
    {       // join 25 pushUpdate threads
        pthread_join(pushUpdateThread[i], NULL);
    }
    pthread_join(userSimulatorThread, NULL);        // join userSimulator thread
    return 0;
}