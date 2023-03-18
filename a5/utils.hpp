#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <signal.h>
#define MAX_PRIORITY 10

using namespace std;

class Guest{

public:
    int id;
    int priority;
    int stay_time;

    Guest& operator=(Guest &g){
        id = g.id;
        priority = g.priority;
        stay_time = g.stay_time;
        return *this;
    }
};

typedef struct _room{
    bool is_occupied;
    Guest guest;
    int time_occupied;
    sem_t sem;
}Room;

typedef struct _hotel{

    vector<Room> rooms;
    int occupancy;
    int num_need_cleaning;
}Hotel;

#endif