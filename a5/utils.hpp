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

typedef struct _guest{

    int id;
    int priority;
    int stay_time;
}Guest;

typedef struct _room{
    bool is_occupied;
    Guest guest;
}Room;

#endif