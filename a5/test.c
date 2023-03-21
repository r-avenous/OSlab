// #include <stdio.h>
// #include <unistd.h>
// #include <pthread.h>
// #include <signal.h>

// void *thread_func(void *arg)
// {
//     int *sleep_time = (int *)arg;

//     printf("Thread sleeping for %d seconds...\n", *sleep_time);

//     // Sleep for the specified time
//     sleep(*sleep_time);

//     printf("Thread woke up.\n");
//     printf("Thread exiting...\n");

//     return NULL;
// }

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

volatile sig_atomic_t flag = 0;

void handle_signal(int sig)
{
    // flag = 1;
}

void *thread_func(void *arg)
{
    int *sleep_time = (int *)arg;

    printf("Thread sleeping for %d seconds...\n", *sleep_time);

    // Set up the signal handler for SIGALRM
    // struct sigaction sa;
    // sa.sa_handler = handle_signal;
    // sigemptyset(&sa.sa_mask);
    // sa.sa_flags = 0;
    // sigaction(SIGALRM, &sa, NULL);
    signal(SIGUSR1, handle_signal);
    // Set an alarm for the specified sleep time
    // alarm(*sleep_time);
    sleep(*sleep_time);
    printf("Thread woke up.\n");

    // Wait for the signal to be received
    // while (!flag) {
    //     // Do some work here if needed
    //     // ...
    // }

    printf("Thread received signal.\n");

    return NULL;
}


int main()
{
    pthread_t tid;
    int sleep_time = 10;

    // Create a new thread
    if (pthread_create(&tid, NULL, thread_func, &sleep_time) != 0) {
        printf("Failed to create thread.\n");
        return 1;
    }

    // Sleep for 1 second
    sleep(4);

    // Interrupt the sleeping thread
    if (pthread_kill(tid, SIGUSR1) != 0) {
        printf("Failed to interrupt thread.\n");
        return 1;
    }

    // Join the thread
    if (pthread_join(tid, NULL) != 0) {
        printf("Failed to join thread.\n");
        return 1;
    }

    printf("Thread joined.\n");

    return 0;
}
