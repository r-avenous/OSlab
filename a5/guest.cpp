#include "guest.hpp"

extern pthread_mutex_t guest_mutex;

void guest(int num)
{
    pthread_mutex_lock(&guest_mutex);
    srand(time(NULL) + num);
    printf("I am guest %d\n", num);
    int sleep_time = rand() % 11 + 10;
    printf("Guest %d will sleep for %d seconds\n", num, sleep_time);
    pthread_mutex_unlock(&guest_mutex);
    sleep(sleep_time);

}