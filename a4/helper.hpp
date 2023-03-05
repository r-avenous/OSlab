#ifndef HELPER_HPP
#define HELPER_HPP

#include <iostream>
#include <fstream>
#include <vector>
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

void printTime(time_t, ostream&);
ostream& operator<<(ostream&, action);

#endif