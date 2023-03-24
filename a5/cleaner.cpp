#include "cleaner.hpp"

extern pthread_mutex_t cleaner_mutex;
extern pthread_mutex_t hotel_mutex;
extern pthread_cond_t clean_cond;
extern int x, y, n;
extern Hotel hotel;

void cleaner(int id)
{
    printf("I am cleaner [%d]\n", id);
    while(1)
    {
        pthread_mutex_lock(&hotel_mutex);
        while(!hotel.is_cleaning) pthread_cond_wait(&clean_cond, &hotel_mutex);
        int t = hotel.cleanRoom();
        pthread_mutex_unlock(&hotel_mutex);
        
        printf("Cleaning room for %d seconds\n", t);
        sleep(PROPORTIONALITY * t);
        printf("Room cleaned\n");

        sem_post(&hotel.clean_rooms_sem);
        sem_post(&hotel.net_occ_sem);
        sem_post(&hotel.net_occ_sem);

        pthread_mutex_lock(&hotel_mutex);
        int curCleanRooms;
        sem_getvalue(&hotel.clean_rooms_sem, &curCleanRooms);
        if(curCleanRooms == n)
        {
            printf("Cleaning is done\n");
            hotel.is_cleaning = false;
        }
        pthread_mutex_unlock(&hotel_mutex);
    }
}