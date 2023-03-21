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
        sem_wait(&hotel.start_cleaning_sem);
        pthread_mutex_lock(&hotel_mutex);
        if(hotel.dirty_and_empty_rooms.size()!=0){
            printf("I am cleaner [%d] and I am cleaning\n", id);
            Room room = hotel.dirty_and_empty_rooms.back();
            hotel.dirty_and_empty_rooms.pop_back();
            room.num_times_occupied = 0;
            int prev_time = room.time_occupied;
            room.time_occupied = 0;
            // guest eviction???
            pthread_mutex_unlock(&hotel_mutex);
            printf("room time occupied = %d", prev_time);
            sleep(PROPORTIONALITY * prev_time);
            pthread_mutex_lock(&hotel_mutex);
            hotel.nondirty_and_empty_rooms.push_back(room);
            if(hotel.dirty_and_empty_rooms.size()==n){
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
        // else{
        //     pthread_mutex_unlock(&hotel_mutex);
        //     continue;
        // }
    }

    // while (!hotel.is_cleaning)
    //     pthread_cond_wait(&clean_cond, &hotel_mutex);

    // // select a random room for cleaning
    // int room_id;
    // while (1){

    //     room_id = rand() % n;
    //     if (hotel.rooms[room_id].is_dirty)
    //         break;
    // }

    // // clean the room
    // int clean_time = PROPORTIONALITY * hotel.rooms[room_id].time_occupied + PROPORTIONALITY;

    // // update the hotel and room details
    // hotel.rooms[room_id].is_dirty = false;
    // hotel.rooms[room_id].time_occupied = 0;
    // hotel.rooms[room_id].num_times_occupied = 0;
    // sem_post(&hotel.clean_rooms_sem);

    // // if cleaners have cleaned all the rooms, set is_cleaning to false
    // int val;
    // sem_getvalue(&hotel.clean_rooms_sem, &val);

    // if (val == n)
    //     hotel.is_cleaning = false;

    // pthread_mutex_unlock(&hotel_mutex);

    // printf("Cleaner [%d] cleaning room [%d] for %d seconds ...\n", id, room_id+1, clean_time);
    // sleep(clean_time);
    
    // printf("Room [%d]!\n is cleaned!", room_id+1);

    // pthread_mutex_unlock(&cleaner_mutex);
}