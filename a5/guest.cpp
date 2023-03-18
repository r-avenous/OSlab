#include "guest.hpp"

void guest(int num)
{
    srand(time(NULL));
    cout << "I am guest " << num << endl;
    int sleep_time = rand() % 11 + 10;
    cout << "Guest " << num << " will sleep for " << sleep_time << " seconds" << endl;
    sleep(sleep_time);
}