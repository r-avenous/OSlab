#include "cleaner.hpp"

extern pthread_mutex_t cleaner_mutex;

void cleaner(int num){
    pthread_mutex_lock(&cleaner_mutex);
    printf("I am cleaner %d\n", num);
    pthread_mutex_unlock(&cleaner_mutex);
}