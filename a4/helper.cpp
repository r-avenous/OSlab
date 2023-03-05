#include "helper.hpp"

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