#include "guest.hpp"

extern pthread_mutex_t guest_mutex;
extern int x, y, n;
extern vector<Room> hotel;

void guest(int id)
{
    pthread_mutex_lock(&guest_mutex);
    srand(time(NULL) + id);

    Guest guest;
    guest.id = id;
    guest.priority = rand() % MAX_PRIORITY + 1;
    printf("I am guest %d\n", id);

    int sleep_time = rand() % 11 + 10;
    printf("Guest %d will sleep for %d seconds\n", id, sleep_time);
    pthread_mutex_unlock(&guest_mutex);

    sleep(sleep_time);

    guest.stay_time = rand() % 21 + 10;
    printf("Guest %d wants to stay for %d seconds\n", id, guest.stay_time);



}