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
        int sleep_time = rand() % 11 + 10; //???????
        printf("Guest [%d] will sleep for %d seconds\n", guest.id, sleep_time);
        sleep(sleep_time);

        guest.stay_time = rand() % 21 + 10; // ???????????
        printf("Guest [%d] wants to stay for %d seconds\n", guest.id, guest.stay_time);

        pthread_mutex_lock(&hotel_mutex);
        if(hotel.is_cleaning)
        {
            pthread_mutex_unlock(&hotel_mutex);
            printf("Guest %d cannot enter due to cleaning\n", guest.id);
            continue;
        }
        
        int curCleanRooms;
        sem_getvalue(&hotel.clean_rooms_sem, &curCleanRooms);
        if(curCleanRooms)
        {
            sem_wait(&hotel.clean_rooms_sem);
            Room r = hotel.getCleanRoom();
            printf("Guest %d allocated room %d with %d priority\n", guest.id, r.room_id, guest.priority);
            hotel.occupy(r, guest);

            int curNetOcc;
            sem_getvalue(&hotel.net_occ_sem, &curNetOcc);
            sem_wait(&hotel.net_occ_sem);
            printf("Occ sem decremented to %d\n", curNetOcc-1);
            if(curNetOcc == 1)
            {
                printf("Evicting all guests\n");
                hotel.is_cleaning = true;
                for(int i=0; i<n; i++)
                {
                    if(guestThread[i] != pthread_self())
                        pthread_kill(guestThread[i], SIGUSR1);
                }
                pthread_cond_broadcast(&clean_cond);
                pthread_mutex_unlock(&hotel_mutex);
                continue;
            }
            pthread_mutex_unlock(&hotel_mutex);

            sleep(guest.stay_time);

            if(r.num_times_occupied == 1)
            {
                sem_post(&hotel.clean_rooms_sem);

                pthread_mutex_lock(&hotel_mutex);
                auto it = hotel.nondirty_and_occupied_rooms.find(r);
                if(it != hotel.nondirty_and_occupied_rooms.end())
                {
                    hotel.nondirty_and_empty_rooms.push_back(r);
                    hotel.nondirty_and_occupied_rooms.erase(it);
                }
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
        auto it = hotel.nondirty_and_occupied_rooms.find(r);
        hotel.nondirty_and_occupied_rooms.erase(it);
        printf("Guest %d allocated room %d with %d priority replaces %d with %d priority\n", guest.id, r.room_id, guest.priority, r.guest.id, r.guest.priority);
        hotel.occupy(r, guest);

        int curNetOcc;
        sem_getvalue(&hotel.net_occ_sem, &curNetOcc);
        sem_wait(&hotel.net_occ_sem);
        sem_wait(&hotel.clean_rooms_sem);
        printf("Occ sem decremented to %d\n", curNetOcc-1);
        if(curNetOcc == 1)
        {
            printf("Evicting all guests\n");
            hotel.is_cleaning = true;
            for(int i=0; i<n; i++)
            {
                if(guestThread[i] != pthread_self())
                    pthread_kill(guestThread[i], SIGUSR1);
            }
            pthread_cond_broadcast(&clean_cond);
            pthread_mutex_unlock(&hotel_mutex);
            continue;
        }
        pthread_mutex_unlock(&hotel_mutex);

        sleep(guest.stay_time);
    }
}