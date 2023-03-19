#include "cleaner.hpp"

extern pthread_mutex_t cleaner_mutex;
extern pthread_mutex_t hotel_mutex;
extern pthread_cond_t clean_cond;
extern int x, y, n;
extern Hotel hotel;

void cleaner(int id){

    srand(time(NULL) + id);
    // pthread_mutex_lock(&cleaner_mutex);
    printf("I am cleaner [%d]\n", id);

    while(1){

        pthread_mutex_lock(&hotel_mutex);
        printf("Cleaner [%d] looking to clean rooms ...\n", id);

        while (!hotel.is_cleaning)
            pthread_cond_wait(&clean_cond, &hotel_mutex);

        // if cleaners have cleaned all the rooms, set is_cleaning to false
        int val;
        sem_getvalue(&hotel.clean_rooms_sem, &val);

        if (val == n)
        {
            if (hotel.is_cleaning)
                hotel.is_cleaning = false;
                
            pthread_mutex_unlock(&hotel_mutex);
            continue;
        }

        // select a random room for cleaning
        int room_id;
        while (1){

            room_id = rand() % n;
            if (hotel.rooms[room_id].is_dirty)
                break;
        }

        // clean the room
        int clean_time = PROPORTIONALITY * hotel.rooms[room_id].time_occupied + PROPORTIONALITY;

        // update the hotel and room details
        hotel.rooms[room_id].is_dirty = false;
        hotel.rooms[room_id].time_occupied = 0;
        hotel.rooms[room_id].num_times_occupied = 0;
        sem_post(&hotel.clean_rooms_sem);
        
        pthread_mutex_unlock(&hotel_mutex);

        printf("Cleaner [%d] cleaning room [%d] for %d seconds ...\n", id, room_id+1, clean_time);
        sleep(clean_time);
        
        printf("Room [%d] is cleaned!\n", room_id+1);

        // pthread_mutex_unlock(&cleaner_mutex);
    }
}