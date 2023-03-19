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

    Guest(){
        id = -1;
        priority = -1;
        stay_time = 0;
    }

    Guest& operator=(Guest &g){
        id = g.id;
        priority = g.priority;
        stay_time = g.stay_time;
        return *this;
    }
};

typedef struct _room{
    bool is_occupied;
    bool is_dirty;
    Guest guest;
    int time_occupied;
    int num_times_occupied;
}Room;

typedef struct _hotel{

    vector<Room> rooms;
    int occupancy;
    sem_t clean_rooms_sem;
    bool is_cleaning;
}Hotel;

#endif