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

enum actionType     //actionType
{
    POST, COMMENT, LIKE
};

typedef struct _action
{
    int userID, actionID, actionType;       
    time_t timeStamp;
    _action(int userID, int actionID, int actionType)       //constructor
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
        Out(string filename = LOGFILE)      //constructor
        {
            log.open(filename, ios::out | ios::app);        //open log file in append mode
        }
        template <typename T>
        Out& operator<<(T const& value)     //overloaded << operator
        {
            cout << value;
            log << value;
            return *this;
        }
        ~Out()      //destructor
        {
            cout << "Log file closed.\n";
            log.close();        //close log file
        }
};
class feedQueue
{
    public:
    int type, userID;
    pthread_mutex_t lock;
    vector<action> feed;
    struct actionCompare
    {
        feedQueue* fq;
        actionCompare()
        {
            fq = nullptr;       //default constructor
        }
        actionCompare(feedQueue* fq)
        {
            this->fq = fq;      //parameterized constructor
        }
        bool operator()(const action& a, const action& b);      //overloaded () operator
    };
    priority_queue<action, vector<action>, actionCompare> pq;
    actionCompare comp;
    feedQueue()
    {
        type = -1;
        userID = -1;
        pthread_mutex_init(&lock, NULL);
        pq = priority_queue<action, vector<action>, actionCompare>(comp);       //default constructor
    }
    feedQueue(int type, int userID)
    {
        this->type = type;
        this->userID = userID;
        pthread_mutex_init(&lock, NULL);        //mutex initialization
        if(type == PRIORITY) comp = actionCompare(this);        //custom comparator for priority queue if type is PRIORITY 
        pq = priority_queue<action, vector<action>, actionCompare>(comp);       //parameterized constructor
    }
    void push(action a)     //push action to queue  depending on type
    {
        if(type == PRIORITY) pq.push(a);
        else feed.push_back(a); 
    }
    bool empty()        //check if queue is empty
    {
        return (type == PRIORITY)? pq.empty(): feed.empty();
    }
    action top()        //return top element of queue depending on type
    {
        if(type == PRIORITY) return pq.top();
        else return feed.back();
    }
    action pop()        //pop top element of queue depending on type
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