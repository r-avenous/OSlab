#include "guest.hpp"

pthread_cond_t clean_cond = PTHREAD_COND_INITIALIZER;

extern pthread_mutex_t guest_mutex;
extern pthread_mutex_t hotel_mutex;
extern int x, y, n;
extern Hotel hotel;

extern pthread_t *guestThread, *cleanerThread;

void handle_signal(int sig){

}

void evict_guests_from_room(Room& room)
{
    room.guest.is_evicted = 1;
    pthread_kill(guestThread[(room.guest.id)-1], SIGUSR1);
}

void is_time_to_clean()
{
    pthread_mutex_lock(&hotel_mutex);
    if((hotel.dirty_and_empty_rooms.size() + hotel.dirty_and_occupied_rooms.size()) == n){
        int size = hotel.dirty_and_occupied_rooms.size();
        for(int i=0; i<size; i++){
            auto it = hotel.dirty_and_occupied_rooms.begin();
            Room room = *it;
            printf("guest %d evicted due to cleaning\n",room.guest.id);
            hotel.dirty_and_occupied_rooms.erase(it);
            //evict_guests(room);
            evict_guests_from_room(room);
            hotel.dirty_and_empty_rooms.push_back(room);
        }
        printf("All rooms are dirty. Cleaning all rooms...\n");
        printf("no of dirty and occupied rooms = %ld, no of dirty and empty rooms = %ld\n", hotel.dirty_and_occupied_rooms.size(), hotel.dirty_and_empty_rooms.size());
        for(int i=0;i<n;i++){
            sem_post(&hotel.start_cleaning_sem);
        }
    int val;
        sem_getvalue(&hotel.clean_rooms_sem, &val);
        printf("sem value = %d\n", val);
    }
    pthread_mutex_unlock(&hotel_mutex);
}

// bool allocate_rooms(Guest guest, int& room_no){
void allocate_rooms(Guest& guest){
    int flag = 0,flag2=0;
    while(1){
        if(flag == 0){
            pthread_mutex_lock(&hotel_mutex);
            // printf("HI I am Guest %d. I got the lock\n. My flag2 value is: %d\n", guest.id, flag2);
            if(hotel.nondirty_and_empty_rooms.size()!=0) {
                flag2 =1;
                // printf("flag2 got changed here1");
            }
            if(!flag2 && (hotel.nondirty_and_occupied_rooms.size()!=0) && (guest.priority>(*hotel.nondirty_and_occupied_rooms.begin()).guest.priority)) {
                flag2 = 1;
                // printf("flag2 got changed here2");
            }
            if(!flag2 && (hotel.dirty_and_occupied_rooms.size()!=0) && (guest.priority>(*hotel.dirty_and_occupied_rooms.begin()).guest.priority)) {
                flag2 = 1;
                // printf("flag2 got changed here3");
            }
            pthread_mutex_unlock(&hotel_mutex);
            // printf("HI I AM guest %d. I got unlocked. Flag2 value = %d\n", guest.id, flag2);
            if(flag2){
                printf("not going into sem wait, Guest %d\n", guest.id);
                flag2 = 0;
            }
            else{
                printf("Going into sem wait, Guest %d\n", guest.id);
                flag = 2;
                sem_wait(&hotel.clean_rooms_sem);
                printf("Guest %d got the semaphore\n", guest.id);
            }
        }
        pthread_mutex_lock(&hotel_mutex);
        if(hotel.nondirty_and_empty_rooms.size()!=0){
            if(flag==2){
                flag = 0;
            }
            else{
                // printf("Going into sem wait, Guest %d\n", guest.id);
                sem_wait(&hotel.clean_rooms_sem);
                printf("Guest %d got the semaphore\n", guest.id);
            }
            Room room = hotel.nondirty_and_empty_rooms.back();
            hotel.nondirty_and_empty_rooms.pop_back();
            printf("Guest %d allocated room %d\n", guest.id, room.room_id);
            int val;
            sem_getvalue(&hotel.clean_rooms_sem, &val);
            printf("semaphore value: %d\n", val);
            room.guest = guest;
            room.num_times_occupied++;
            room.time_occupied += guest.stay_time; 
            if(room.num_times_occupied==1){
                hotel.nondirty_and_occupied_rooms.insert(room);
            }
            else{
                // sem_wait(&hotel.clean_rooms_sem);
                hotel.dirty_and_occupied_rooms.insert(room);
            }
            pthread_mutex_unlock(&hotel_mutex);
            // flag = 1;
            if(room.num_times_occupied==2){
                is_time_to_clean();
            }
            printf("Guest %d is occupying room\n", guest.id);
            sleep(guest.stay_time);
            if(guest.is_evicted==1){
                printf("Guest %d is evicted\n", guest.id);
                guest.is_evicted = 0;
                return;
            }
            if(room.num_times_occupied==1){
                pthread_mutex_lock(&hotel_mutex);
                printf("Guest %d is checking out\n", guest.id);
                auto it = hotel.nondirty_and_occupied_rooms.find(room);
                // printf("%d\n",(it == hotel.nondirty_and_occupied_rooms.end()));
                hotel.nondirty_and_occupied_rooms.erase(it);
                hotel.nondirty_and_empty_rooms.push_back(room);
                pthread_mutex_unlock(&hotel_mutex);
                printf("now posting to clean rooms\n");
                sem_post(&hotel.clean_rooms_sem);
            }
            else if(room.num_times_occupied==2){
                pthread_mutex_lock(&hotel_mutex);
                auto it = hotel.dirty_and_occupied_rooms.find(room);
                hotel.dirty_and_occupied_rooms.erase(room);
                // evict_guests_from_room(room);
                printf("This room is dirty and occupied. Pushing it to dirty and empty rooms\n");
                hotel.dirty_and_empty_rooms.push_back(room);
                pthread_mutex_unlock(&hotel_mutex);
                // hotel.dirty_and_empty_rooms.push_back(room);
            }
            return;
        }
        if((hotel.nondirty_and_occupied_rooms.size()!=0) && (guest.priority>(*hotel.nondirty_and_occupied_rooms.begin()).guest.priority)){
            auto it = hotel.nondirty_and_occupied_rooms.begin();
            Room room = *it;
            hotel.nondirty_and_occupied_rooms.erase(it);
            room.num_times_occupied++;
            room.time_occupied += guest.stay_time;
            printf("guest %d evicted and should be added\n", room.guest.id);
            evict_guests_from_room(room);
            room.guest = guest;
            hotel.dirty_and_occupied_rooms.insert(room);
            printf("Guest %d allocated room\n", guest.id);
            // sem_wait(&hotel.clean_rooms_sem);
            pthread_mutex_unlock(&hotel_mutex);
            is_time_to_clean();
            sleep(guest.stay_time);
            if(guest.is_evicted == 1){
                guest.is_evicted = 0;
                return;
            }
            return;
            // sem_post(&hotel.clean_rooms_sem);
            // continue;
        }
        if((hotel.dirty_and_occupied_rooms.size()!=0) && (guest.priority>(*hotel.dirty_and_occupied_rooms.begin()).guest.priority)){
            auto it = hotel.dirty_and_occupied_rooms.begin();
            Room room = *it;
            hotel.dirty_and_occupied_rooms.erase(it);
            // room.guest = NULL;
            printf("guest %d evicted and not added\n", room.guest.id);
            evict_guests_from_room(room);
            hotel.dirty_and_empty_rooms.push_back(room);
            pthread_mutex_unlock(&hotel_mutex);
            // is_time_to_clean();
            flag = 1;
            sem_wait(&hotel.clean_rooms_sem);
        }
        if(flag==1){
            flag = 2;
            continue;
        }
        pthread_mutex_unlock(&hotel_mutex);
        flag = 2;
        printf("Going in sem wait\n");
        // sem_wait(&hotel.clean_rooms_sem);
        printf("Came out of sem wait\n");
    }

}

void guest(int id, int priority)
{
    signal(SIGUSR1, handle_signal);
    srand(time(NULL) + id);

    Guest guest;
    guest.id = id;
    // guest.priority = rand() % MAX_PRIORITY + 1;
    guest.priority = priority;
    guest.is_evicted = 0;
    printf("I am guest [%d] with %d priority!\n", guest.id, guest.priority);


    while (1)
    {        
        int sleep_time = rand() % 11 + 1; //???????
        printf("Guest [%d] will sleep for %d seconds\n", guest.id, sleep_time);
        sleep(sleep_time);

        guest.stay_time = rand() % 21 + 1; // ???????????
        printf("Guest [%d] wants to stay for %d seconds\n", guest.id, guest.stay_time);

        pthread_mutex_lock(&hotel_mutex);
        int curCleanRooms = sem_getvalue(&hotel.clean_rooms_sem, NULL);
        if(curCleanRooms)
        {
            sem_wait(&hotel.clean_rooms_sem);
            Room r = hotel.getCleanRoom();
            printf("Guest %d allocated room %d with %d priority\n", guest.id, r.room_id, guest.priority);
            hotel.occupy(r, guest);
            pthread_mutex_unlock(&hotel_mutex);

            sleep(guest.stay_time);

            if(r.num_times_occupied == 1)
            {
                sem_post(&hotel.clean_rooms_sem);

                pthread_mutex_lock(&hotel_mutex);
                auto it = hotel.nondirty_and_occupied_rooms.find(r);
                hotel.nondirty_and_empty_rooms.push_back(r);
                hotel.nondirty_and_occupied_rooms.erase(it);
                pthread_mutex_unlock(&hotel_mutex);
            }
            continue;
        }
        Room r = hotel.getLeastPriorityRoom();
        if(r.room_id == -1 || r.guest.priority > guest.priority)
        {
            pthread_mutex_unlock(&hotel_mutex);

            printf("Guest %d cannot evict anyone else\n", guest.id);
            continue;
        }
        evict_guests_from_room(r);
        printf("Guest %d allocated room %d with %d priority replaces %d with %d priority\n", guest.id, r.room_id, guest.priority, r.guest.id, r.guest.priority);
        hotel.occupy(r, guest);
        pthread_mutex_unlock(&hotel_mutex);

        sleep(guest.stay_time);
    }
}