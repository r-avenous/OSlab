#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <set>

#define PRIORITY 0
#define CHRONOLOGICAL 1

using namespace std;

char filename[] = "musae_git_edges.csv";
int n;
unordered_map<int, vector<int>> graph;
vector<int> type;

int main()
{
    // read csv file and store in array
    ifstream file(filename);
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
    return 0;
}