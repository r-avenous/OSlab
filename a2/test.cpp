#include <iostream>
#include <fstream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>
#include <fnmatch.h>
#include <fcntl.h>
#include <pthread.h>

using namespace std;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int i;

int main(){

	pthread_mutex_lock(&mutex);
	while (i == 0){
		cout << "Waiting on 0" << endl;
		pthread_cond_wait(&cond, &mutex);
	}
	
	if (fork() == 0){
		pthread_mutex_lock(&mutex);
		i = 1;
		cout << "Incremented" << endl;
		pthread_cond_signal(&cond);
		pthread_mutex_unlock(&mutex);
		exit(0);
	}
	pthread_mutex_unlock(&mutex);
	return 0;
}
