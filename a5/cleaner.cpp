#include "cleaner.hpp"

extern pthread_mutex_t cleaner_mutex;
extern pthread_mutex_t hotel_mutex;
extern pthread_cond_t clean_cond;
extern int x, y, n;
extern Hotel hotel;

void cleaner(int id)
{
    srand(time(NULL) + id);
    // pthread_mutex_lock(&cleaner_mutex);
    printf("I am cleaner [%d]\n", id);
    while(1)
    {
        // sem_wait(&hotel.start_cleaning_sem);
        pthread_mutex_lock(&hotel_mutex);
        if(hotel.dirty_and_empty_rooms.size()!=0){
            printf("I am cleaner [%d] and I am cleaning and size of array is %ld\n", id, hotel.dirty_and_empty_rooms.size());
            Room room = hotel.dirty_and_empty_rooms.back();
            hotel.dirty_and_empty_rooms.pop_back();
            room.num_times_occupied = 0;
            int prev_time = room.time_occupied;
            room.time_occupied = 0;
            // guest eviction???
            pthread_mutex_unlock(&hotel_mutex);
            printf("room to be cleaned = %d time occupied = %d\n",room.room_id, (int)(prev_time*PROPORTIONALITY));
            sleep((int)(PROPORTIONALITY * prev_time));
            pthread_mutex_lock(&hotel_mutex);
            hotel.nondirty_and_empty_rooms.push_back(room);
            if(hotel.nondirty_and_empty_rooms.size()==n){
                printf("Finished cleaning all rooms\n\n");
                for(int i=0;i<n;i++){
                    sem_post(&hotel.clean_rooms_sem);
                }
                pthread_mutex_unlock(&hotel_mutex);
                continue;
            }
            else{
                pthread_mutex_unlock(&hotel_mutex);
                continue;
            }
        }
    }
}