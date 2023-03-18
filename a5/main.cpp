#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include "cleaner.hpp"
#include "guest.hpp"

using namespace std;

void* cleaner_func(void* arg){

    int cleaner_num = *(int*)arg;
    // printf("I am cleaner func %d\n", cleaner_num);
    cleaner(cleaner_num);
    return NULL;
}

void* guest_func(void* arg){
    
    int guest_num = *(int*)arg;
    // printf("I am guest func %d\n", guest_num);
    guest(guest_num);
    return NULL;
}

int main(int argc, char* argv[]){

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);
    int n = atoi(argv[3]);

    pthread_t cleanerThread[x], guestThread[y];
    for (int i=0; i<x; i++){
        int *temp = new int(i+1);
        pthread_create(&cleanerThread[i], NULL, cleaner_func, (void*)temp);
    }

    for (int i=0; i<y; i++){
        int *temp = new int(i+1);
        pthread_create(&guestThread[i], NULL, guest_func, (void*)temp);
    }

    for (int i=0; i<x; i++){
        pthread_join(cleanerThread[i], NULL);
    }

    for (int i=0; i<y; i++){
        pthread_join(guestThread[i], NULL);
    }
    return 0;
}