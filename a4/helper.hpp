#ifndef HELPER_HPP
#define HELPER_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <unordered_map>
#include <set>
#include <pthread.h>
#include <time.h>
#include <fstream>
#include <signal.h>

#define PRIORITY 0
#define CHRONOLOGICAL 1
#define LOGFILE "sns.log"
#define GRAPHFILE "musae_git_edges.csv"

using namespace std;

enum actionType
{
    POST, COMMENT, LIKE
};

typedef struct _action
{
    int userID, actionID, actionType;
    time_t timeStamp;
    _action(int userID, int actionID, int actionType)
    {
        this->userID = userID;
        this->actionID = actionID;
        this->actionType = actionType;
        this->timeStamp = time(NULL);
    }
}action;

class Out
{
    ofstream log;
    public:
        Out(string filename = LOGFILE)
        {
            log.open(filename);
        }
        template <typename T>
        Out& operator<<(T const& value)
        {
            cout << value;
            log << value;
            return *this;
        }
        ~Out()
        {
            cout << "Log file closed.\n";
            log.close();
        }
};
struct actionCompare
{
    bool operator()(const action& a, const action& b);
};
class feedQueue
{
    public:
    int type, userID;
    pthread_mutex_t lock;
    vector<action> feed;
    priority_queue<action, vector<action>, actionCompare> pq;
    feedQueue()
    {
        type = -1;
        userID = -1;
        pthread_mutex_init(&lock, NULL);
    }
    feedQueue(int type, int userID)
    {
        this->type = type;
        this->userID = userID;
        pthread_mutex_init(&lock, NULL);
    }
    void push(action a)
    {
        if(type == PRIORITY) pq.push(a);
        else feed.push_back(a);
    }
    bool empty()
    {
        return (type == PRIORITY)? pq.empty(): feed.empty();
    }
    action top()
    {
        if(type == PRIORITY) return pq.top();
        else return feed.back();
    }
    action pop()
    {
        if(type == PRIORITY)
        {
            action a = pq.top();
            pq.pop();
            return a;
        }
        else
        {
            action a = feed.back();
            feed.pop_back();
            return a;
        }
    }
};
void printTime(time_t, ostream&);
ostream& operator<<(ostream&, action);

#endif