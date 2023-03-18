#include "guest.hpp"

extern pthread_mutex_t guest_mutex;
extern pthread_mutex_t hotel_mutex;
extern int x, y, n;
extern Hotel hotel;

bool allocate_rooms(Guest guest){

    pthread_mutex_lock(&hotel_mutex);
    printf("Current hotel occupancy: %d\n", hotel.occupancy);
    bool room_found = false;

    if (hotel.occupancy < n)
    {
        for (int i=0; i<n; i++)
        {
            int val;
            sem_getvalue(&hotel.rooms[i].sem, &val);

            if (!hotel.rooms[i].is_occupied && val > 0)
            {
                room_found = true;
                hotel.rooms[i].is_occupied = true;
                hotel.rooms[i].guest = guest;
                hotel.rooms[i].time_occupied += guest.stay_time;
                sem_wait(&hotel.rooms[i].sem);
                sem_getvalue(&hotel.rooms[i].sem, &val);

                if (val == 0)
                {
                    hotel.num_need_cleaning++;
                    printf("Room [%d] needs cleaning!\n", i+1);
                }
                hotel.occupancy++;
                printf("Allocated room [%d] to Guest [%d]!\n", i+1, guest.id);
                break;
            }
        }
    }
    else{
        printf("No rooms vacant for Guest [%d] ! Looking to displace low priority guests ...\n", guest.id);

        for (int i=0; i<n; i++)
        {
            int val;
            sem_getvalue(&hotel.rooms[i].sem, &val);

            if (hotel.rooms[i].guest.priority < guest.priority && val > 0)
            {
                room_found = true;
                printf("Displacing Guest [%d] with low priority [%d] from room [%d] ...\n", hotel.rooms[i].guest.id, hotel.rooms[i].guest.priority, i+1);
                hotel.rooms[i].guest = guest;
                hotel.rooms[i].time_occupied += guest.stay_time;
                sem_wait(&hotel.rooms[i].sem);
                sem_getvalue(&hotel.rooms[i].sem, &val);

                if (val == 0)
                {
                    hotel.num_need_cleaning++;
                    printf("Room [%d] needs cleaning!\n", i+1);
                }

                printf("Allocated room [%d] to Guest [%d]!\n", i+1, guest.id);
                break;
            }
        }
    }

    if (!room_found)
        printf("No low priority guests found! Guest [%d] will be removed!\n", guest.id);

    pthread_mutex_unlock(&hotel_mutex);
    return room_found;
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

        pthread_mutex_lock(&guest_mutex);

        if (!allocate_rooms(guest))
            printf("Guest [%d] must leave the hotel! Sleeping for %d seconds ...\n", guest.id, (int)guest.stay_time);
        
        else    
            printf("Guest [%d] will now sleep in allotted room for %d seconds ...\n", guest.id, (int)guest.stay_time);

        pthread_mutex_unlock(&guest_mutex);

        sleep(guest.stay_time);
    }
}