#include "helper.hpp"
#include "userSimulator.hpp"
// #include "readPost.hpp"
#include "pushUpdate.hpp"

int n;
unordered_map<int, vector<int>> graph;
vector<int> type;
Out out;
unordered_map<int, int> counter;
unordered_map<int, vector<action>> wallQueue, feedQueue;

void sig_handler(int signo)
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
    file.close();
    n = nodes.size();
    nodes.clear();
    type.assign(n, 0);
    for(int i=0; i<n; i++)
    {
        // assign random type 0 or 1
        type[i] = rand() % 2;
    }
    pthread_t userSimulatorThread, readPostThread[10], pushUpdateThread[25];
    pthread_create(&userSimulatorThread, NULL, userSimulator, NULL);
    for(int i=0; i<25; i++)
    {
        pthread_create(&pushUpdateThread[i], NULL, pushUpdate, (void*)&i);
    }
    // for(int i=0; i<10; i++)
    // {
    //     pthread_create(&readPostThread[i], NULL, readPost, (void*)&i);
    // }
    // for(int i=0; i<10; i++)
    // {
    //     pthread_join(readPostThread[i], NULL);
    // }
    for(int i=0; i<25; i++)
    {
        pthread_join(pushUpdateThread[i], NULL);
    }
    pthread_join(userSimulatorThread, NULL);
    return 0;
}