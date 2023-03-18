#include "guest.hpp"
#include "cleaner.hpp"

pthread_mutex_t cleaner_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t guest_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t hotel_mutex = PTHREAD_MUTEX_INITIALIZER;
int x, y, n;
Hotel hotel;

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        cout << "\nSIGINT received\nExiting ...\n";
        exit(0);
    }
}

void* cleaner_func(void* arg){

    int cleaner_id = *(int*)arg;
    cleaner(cleaner_id);
    return NULL;
}

void* guest_func(void* arg){

    int guest_id = *(int*)arg;
    guest(guest_id);
    return NULL;
}

int main(int argc, char* argv[]){

    signal(SIGINT, sig_handler);
    x = atoi(argv[1]);
    y = atoi(argv[2]);
    n = atoi(argv[3]);

    printf("Creating the hotel with %d rooms ...\n", n);
    for (int i=0; i<n; i++)
    {
        Room room;
        room.is_occupied = false;
        room.time_occupied = 0;
        sem_init(&room.sem, 0, 2);
        hotel.rooms.push_back(room);
    }

    hotel.occupancy = 0;
    hotel.num_need_cleaning = 0;
    printf("Initial hotel occupancy: %d\n", hotel.occupancy);

    pthread_t cleanerThread[x], guestThread[y];
    printf("Creating guest and cleaner threads ...\n");

    for (int i=0; i<x; i++){
        int *temp = new int(i+1);
        pthread_create(&cleanerThread[i], NULL, cleaner_func, (void*)temp);
    }

    for (int i=0; i<y; i++){
        int *temp = new int(i+1);
        pthread_create(&guestThread[i], NULL, guest_func, (void*)temp);
    }

    printf("Joining guest and cleaner threads ...\n");
    for (int i=0; i<x; i++){
        pthread_join(cleanerThread[i], NULL);
    }

    for (int i=0; i<y; i++){
        pthread_join(guestThread[i], NULL);
    }
    printf("Exiting ...\n");
    return 0;
}