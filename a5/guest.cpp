#include "guest.hpp"

pthread_cond_t clean_cond = PTHREAD_COND_INITIALIZER;

extern pthread_mutex_t guest_mutex;
extern pthread_mutex_t hotel_mutex;
extern int x, y, n;
extern Hotel hotel;

extern pthread_t *guestThread, *cleanerThread;

void handle_signal(int sig){

}

void evict_guests_from_room(Room& room){
    room.guest.is_evicted = 1;
    pthread_kill(guestThread[(room.guest.id)-1], SIGUSR1);
    // printf("\n\nEvicting guests from their rooms for cleaning purposes...\n\n");
    // for (int i=0; i<n; i++)
    // {
    //     // replace the guest with a dummy guest
    //     hotel.rooms[i].is_occupied = false;
    //     Guest guest;
    //     guest.id = -1;
    //     guest.priority = -1;
    //     guest.stay_time = 0;
    //     hotel.rooms[i].guest = guest;
    //     hotel.occupancy--;
    // }
    // printf("\n\nAll guests evicted! Current occupancy : %d\n\n", hotel.occupancy);
}

void is_time_to_clean(){
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
    // if(hotel.dirty_and_occupied_rooms.size()==0){
    //     hotel.is_cleaning = false;
    //     pthread_mutex_unlock(&hotel_mutex);
    //     return;
    // }
    // Room room = hotel.dirty_and_occupied_rooms.back();
    // hotel.dirty_and_occupied_rooms.pop_back();
    // hotel.nondirty_and_empty_rooms.push_back(room);
    // sem_post(&hotel.clean_rooms_sem);
    // pthread_mutex_unlock(&hotel_mutex);
    // sleep(1);
    // is_time_to_clean();
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

    // pthread_mutex_lock(&hotel_mutex);
    // printf("Current hotel occupancy: %d\n", hotel.occupancy);
    // bool room_found = false;

    // int val;
    // sem_getvalue(&hotel.clean_rooms_sem, &val);
    // printf("Semaphore value : %d\n", val);

    // // if all rooms of the hotel have at least two guests, it needs cleaning - so no rooms are allocated
    // if (val == 0){

    //     printf("Inside val condition\n");
    //     // THIS PART NEEDS TO RUN ONLY ONCE, FOR THE FIRST TIME ALL ROOMS NEED CLEANING !!
    //     if (!hotel.is_cleaning)
    //     {      
    //         hotel.is_cleaning = true;
    //         printf("\n\nAll rooms need cleaning!\n\n");
    //         evict_guests();
    //         printf("\nCleaning staff will be notified!\n");
    //         pthread_cond_signal(&clean_cond);
    //     }
    //     room_no = -1;
    //     pthread_mutex_unlock(&hotel_mutex);
    //     printf("Returning no room found!\n");
    //     return false;
    // }

    // if (hotel.occupancy < n)
    // {
    //     printf("*****1st condition*****\n");
    //     for (int i=0; i<n; i++)
    //     {
    //         if (!hotel.rooms[i].is_occupied)
    //         {
    //             room_found = true;
    //             hotel.rooms[i].is_occupied = true;
    //             hotel.rooms[i].guest = guest;
    //             hotel.rooms[i].num_times_occupied++;

    //             if (hotel.rooms[i].num_times_occupied >= 2 && !hotel.rooms[i].is_dirty)
    //             {
    //                 hotel.rooms[i].is_dirty = true;
    //                 sem_wait(&hotel.clean_rooms_sem);
    //                 printf("Room [%d] needs cleaning!\n", i+1);
    //             }
    //             hotel.occupancy++;
    //             printf("Occupancy : %d\n", hotel.occupancy);
    //             printf("Allocated room [%d] to Guest [%d]!\n", i+1, guest.id);
    //             room_no = i+1;
    //             break;
    //         }
    //     }
    // }
    // else{
    //     printf("No rooms vacant for Guest [%d] ! Looking to displace low priority guests ...\n", guest.id);

    //     for (int i=0; i<n; i++)
    //     {
    //         if (hotel.rooms[i].guest.priority < guest.priority)
    //         {
    //             room_found = true;
    //             printf("Displacing Guest [%d] with low priority [%d] from room [%d] ...\n", hotel.rooms[i].guest.id, hotel.rooms[i].guest.priority, i+1);
    //             hotel.rooms[i].guest = guest;
    //             hotel.rooms[i].num_times_occupied++;
            
    //             if (hotel.rooms[i].num_times_occupied >= 2 && !hotel.rooms[i].is_dirty)
    //             {
    //                 hotel.rooms[i].is_dirty = true;
    //                 sem_wait(&hotel.clean_rooms_sem);
    //                 printf("Room [%d] needs cleaning!\n", i+1);
    //             }

    //             printf("Allocated room [%d] to Guest [%d]!\n", i+1, guest.id);
    //             room_no = i+1;
    //             break;
    //         }
    //     }
    // }

    // if (!room_found){
    //     printf("No low priority guests found! Guest [%d] will be removed!\n", guest.id);
    //     room_no = 0;
    // }

    // pthread_mutex_unlock(&hotel_mutex);
    // return room_found;
}

// void stay_in_room(Guest &guest, int room_no)
// {
//     printf("Guest [%d] will now sleep in room [%d] for %d seconds ...\n", guest.id, room_no, (int)guest.stay_time);
//     sleep(guest.stay_time);
    
//     pthread_mutex_lock(&hotel_mutex);
//     // After guest stay is complete, update the hotel and room details
//     guest.stay_time = 0;
//     hotel.rooms[room_no-1].is_occupied = false;
//     hotel.rooms[room_no-1].time_occupied += guest.stay_time;
//     hotel.rooms[room_no-1].num_times_occupied++;

//     // replace the guest with a dummy guest
//     Guest g;
//     g.id = -1;
//     g.priority = -1;
//     g.stay_time = 0;
//     hotel.rooms[room_no-1].guest = g;
    
//     pthread_mutex_unlock(&hotel_mutex);
// }

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

        // pthread_mutex_lock(&guest_mutex);
        int room_no;
        // int alloc_status = allocate_rooms(guest, room_no);
        allocate_rooms(guest);

        // printf("Room number allotted : %d\n", room_no);
        // if (alloc_status == false && room_no == -1)
        // {
        //     printf("Guest [%d] will have to wait for cleaning to be done!\n", guest.id);
        //     sleep(guest.stay_time);
        // }
        // else if (alloc_status == false && room_no == 0)
        // {
        //     printf("Guest [%d] must leave the hotel! Sleeping for %d seconds ...\n", guest.id, (int)guest.stay_time);
        //     sleep(guest.stay_time);
        // }
        // else if (alloc_status == true)
        //     stay_in_room(guest, room_no);

        // pthread_mutex_unlock(&guest_mutex);
    }
}