#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/wait.h>
#include <iostream>

using namespace std;

int main(int argc, char *argv[]) {

    int fd;
    pid_t pid;
    char *file_to_lock = argv[1];

    // Open the file in read/write mode
    fd = open(file_to_lock, O_RDWR);

    if (fd < 0) {
        perror("Error opening file");
        return 1;
    }

    pid = fork();

    if (pid == 0){

        // Apply lock on the file
        if (flock(fd, LOCK_EX) < 0) {
            perror("Error locking file");
            return 1;
        }

        // Write to the file
        const char *buf = "Hello World!\n"; 
        while (1){

            // cout << "Hello World!" << endl;
            write(fd, buf, strlen(buf));
            sleep(1);
        }
        exit(0);
    }

    int status;
    wait(&status);

    cout << "Child exited with status " << status << endl;
    // Close the file
    close(fd);

    return 0;
}
