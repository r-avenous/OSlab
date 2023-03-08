#include "helper.hpp"

extern unordered_map<int, vector<int>> graph;
void printTime(time_t t, ostream& outf = cout)
{
    struct tm * timeinfo;
    timeinfo = localtime(&t);
    outf << asctime(timeinfo);
}
ostream& operator<<(ostream& outf, action a)
{
    outf << "User " << a.userID << " (" << a.actionID << ") " << (a.actionType == POST ? "posted" : (a.actionType == COMMENT ? "commented" : "liked")) << " at ";
    printTime(a.timeStamp, outf); 
    return outf;
}

bool actionCompare::operator()(const action& a, const action& b)
{
    set<int> neighbours(graph[a.userID].begin(), graph[a.userID].end());
    int aScore = 0, bScore = 0;
    for(int aNeighbours: graph[a.userID])
    {
        if(neighbours.find(aNeighbours) != neighbours.end()) aScore++;
    }
    for(int bNeighbours: graph[b.userID])
    {
        if(neighbours.find(bNeighbours) != neighbours.end()) bScore++;
    }
    return aScore < bScore;
}