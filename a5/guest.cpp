#include "guest.hpp"

pthread_cond_t clean_cond = PTHREAD_COND_INITIALIZER;

extern pthread_mutex_t guest_mutex;
extern pthread_mutex_t hotel_mutex;
extern int x, y, n;
extern Hotel hotel;

void evict_guests(){

    printf("\n\nEvicting guests from their rooms for cleaning purposes...\n\n");
    for (int i=0; i<n; i++)
    {
        // replace the guest with a dummy guest
        hotel.rooms[i].is_occupied = false;
        Guest guest;
        guest.id = -1;
        guest.priority = -1;
        guest.stay_time = 0;
        hotel.rooms[i].guest = guest;
        hotel.occupancy--;
    }
    printf("\n\nAll guests evicted! Current occupancy : %d\n\n", hotel.occupancy);
}

bool allocate_rooms(Guest guest, int& room_no){

    pthread_mutex_lock(&hotel_mutex);
    printf("Current hotel occupancy: %d\n", hotel.occupancy);
    bool room_found = false;

    int val;
    sem_getvalue(&hotel.clean_rooms_sem, &val);
    printf("Semaphore value : %d\n", val);

    // if all rooms of the hotel have at least two guests, it needs cleaning - so no rooms are allocated
    if (val == 0){

        printf("Inside val condition\n");
        // THIS PART NEEDS TO RUN ONLY ONCE, FOR THE FIRST TIME ALL ROOMS NEED CLEANING !!
        if (!hotel.is_cleaning)
        {      
            hotel.is_cleaning = true;
            printf("\n\nAll rooms need cleaning!\n\n");
            evict_guests();
            printf("\nCleaning staff will be notified!\n");
            pthread_cond_signal(&clean_cond);
        }
        room_no = -1;
        pthread_mutex_unlock(&hotel_mutex);
        printf("Returning no room found!\n");
        return false;
    }

    if (hotel.occupancy < n)
    {
        printf("*****1st condition*****\n");
        for (int i=0; i<n; i++)
        {
            if (!hotel.rooms[i].is_occupied)
            {
                room_found = true;
                hotel.rooms[i].is_occupied = true;
                hotel.rooms[i].guest = guest;
                hotel.rooms[i].num_times_occupied++;

                if (hotel.rooms[i].num_times_occupied >= 2 && !hotel.rooms[i].is_dirty)
                {
                    hotel.rooms[i].is_dirty = true;
                    sem_wait(&hotel.clean_rooms_sem);
                    printf("Room [%d] needs cleaning!\n", i+1);
                }
                hotel.occupancy++;
                printf("Occupancy : %d\n", hotel.occupancy);
                printf("Allocated room [%d] to Guest [%d]!\n", i+1, guest.id);
                room_no = i+1;
                break;
            }
        }
    }
    else{
        printf("No rooms vacant for Guest [%d] ! Looking to displace low priority guests ...\n", guest.id);

        for (int i=0; i<n; i++)
        {
            if (hotel.rooms[i].guest.priority < guest.priority)
            {
                room_found = true;
                printf("Displacing Guest [%d] with low priority [%d] from room [%d] ...\n", hotel.rooms[i].guest.id, hotel.rooms[i].guest.priority, i+1);
                hotel.rooms[i].guest = guest;
                hotel.rooms[i].num_times_occupied++;
            
                if (hotel.rooms[i].num_times_occupied >= 2 && !hotel.rooms[i].is_dirty)
                {
                    hotel.rooms[i].is_dirty = true;
                    sem_wait(&hotel.clean_rooms_sem);
                    printf("Room [%d] needs cleaning!\n", i+1);
                }

                printf("Allocated room [%d] to Guest [%d]!\n", i+1, guest.id);
                room_no = i+1;
                break;
            }
        }
    }

    if (!room_found){
        printf("No low priority guests found! Guest [%d] will be removed!\n", guest.id);
        room_no = 0;
    }

    pthread_mutex_unlock(&hotel_mutex);
    return room_found;
}

void stay_in_room(Guest &guest, int room_no)
{
    printf("Guest [%d] will now sleep in room [%d] for %d seconds ...\n", guest.id, room_no, (int)guest.stay_time);
    sleep(guest.stay_time);
    
    pthread_mutex_lock(&hotel_mutex);
    // After guest stay is complete, update the hotel and room details
    guest.stay_time = 0;
    hotel.rooms[room_no-1].is_occupied = false;
    hotel.rooms[room_no-1].time_occupied += guest.stay_time;
    hotel.rooms[room_no-1].num_times_occupied++;

    // replace the guest with a dummy guest
    Guest g;
    g.id = -1;
    g.priority = -1;
    g.stay_time = 0;
    hotel.rooms[room_no-1].guest = g;
    
    pthread_mutex_unlock(&hotel_mutex);
}

void guest(int id)
{
    srand(time(NULL) + id);

    Guest guest;
    guest.id = id;
    guest.priority = rand() % MAX_PRIORITY + 1;
    printf("I am guest [%d] with %d priority!\n", guest.id, guest.priority);

    int sleep_time = rand() % 11 + 10;
    printf("Guest [%d] will sleep for %d seconds\n", guest.id, sleep_time);
    sleep(sleep_time);

    while (1)
    {        
        guest.stay_time = rand() % 21 + 10;
        printf("Guest [%d] wants to stay for %d seconds\n", guest.id, guest.stay_time);

        // pthread_mutex_lock(&guest_mutex);
        int room_no;
        int alloc_status = allocate_rooms(guest, room_no);

        printf("Room number allotted : %d\n", room_no);
        if (alloc_status == false && room_no == -1)
        {
            printf("Guest [%d] will have to wait for cleaning to be done!\n", guest.id);
            sleep(guest.stay_time);
        }
        else if (alloc_status == false && room_no == 0)
        {
            printf("Guest [%d] must leave the hotel! Sleeping for %d seconds ...\n", guest.id, (int)guest.stay_time);
            sleep(guest.stay_time);
        }
        else if (alloc_status == true)
            stay_in_room(guest, room_no);

        // pthread_mutex_unlock(&guest_mutex);

    }
}