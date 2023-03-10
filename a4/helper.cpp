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
unordered_map<int, unordered_map<int, int>> actionCompareCache;         //cache for actionCompare ; stores common neighbours of a user with the user whose feed queue is being updated
bool feedQueue::actionCompare::operator()(const action& a, const action& b)
{
    int aScore = 0, bScore = 0;
    if(actionCompareCache[fq->userID].find(a.userID) != actionCompareCache[fq->userID].end())
    {
        aScore = actionCompareCache[fq->userID][a.userID];
    }
    else
    {
        set<int> neighbours(graph[fq->userID].begin(), graph[fq->userID].end());
        for(int aNeighbours: graph[a.userID])
        {
            if(neighbours.find(aNeighbours) != neighbours.end()) aScore++;
        }
        actionCompareCache[fq->userID][a.userID] = aScore;
        actionCompareCache[a.userID][fq->userID] = aScore;
    }
    if(actionCompareCache[fq->userID].find(b.userID) != actionCompareCache[fq->userID].end())
    {
        bScore = actionCompareCache[fq->userID][b.userID];
    }
    else
    {
        set<int> neighbours(graph[fq->userID].begin(), graph[fq->userID].end());
        for(int bNeighbours: graph[b.userID])
        {
            if(neighbours.find(bNeighbours) != neighbours.end()) bScore++;
        }
        actionCompareCache[fq->userID][b.userID] = bScore;
        actionCompareCache[b.userID][fq->userID] = bScore;
    }
    return aScore < bScore;     //return true if aScore < bScore
}