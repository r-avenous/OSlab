#include "guest.hpp"
#include "cleaner.hpp"

// pthread_mutex_t cleaner_mutex = PTHREAD_MUTEX_INITIALIZER;
// pthread_mutex_t guest_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t hotel_mutex = PTHREAD_MUTEX_INITIALIZER;
int x, y, n;
Hotel hotel;

void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        printf("\nSIGINT received");
        printf("\nDestroying mutexes ...");

        pthread_mutex_destroy(&hotel_mutex);
        // pthread_mutex_destroy(&guest_mutex);
        // pthread_mutex_destroy(&cleaner_mutex);

        printf("\nExiting ...\n");
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
        room.num_times_occupied = 0;
        hotel.rooms.push_back(room);
    }

    hotel.occupancy = 0;
    hotel.is_cleaning = false;
    sem_init(&hotel.clean_rooms_sem, 0, n);
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

    printf("\nDestroying mutexes ...");
    pthread_mutex_destroy(&hotel_mutex);
    // pthread_mutex_destroy(&guest_mutex);
    // pthread_mutex_destroy(&cleaner_mutex);
    printf("\nExiting ...\n");
    return 0;
}