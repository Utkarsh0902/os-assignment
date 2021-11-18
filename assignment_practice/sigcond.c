#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include<semaphore.h>
#include <stdbool.h>

pthread_mutex_t mutex;
bool finished[3] = {false, false, false};

void *func1(void *arg){
     //child process
    for(int i = 0 ; i< 10; i++){
        
        pthread_mutex_lock(&mutex);
        printf("Child: %d\n", i);
        
        usleep(500000); // every half second
        pthread_mutex_unlock(&mutex);
        usleep(5000);
    }
    pthread_mutex_lock(&mutex);
    printf("Done\n");
    finished[0] = true;
    pthread_mutex_unlock(&mutex);
}
int main(){
    pthread_t tid1;
    pthread_create(&tid1,NULL,func1,NULL);
    // initiate the lock
    pthread_mutex_init(&mutex, NULL);

    while(1){
        // lock the mutex : The thread cannot work
        pthread_mutex_lock(&mutex);
        printf("Sleeping for 2 seconds\n");
        sleep(2);
        //unock the mutex: Let the thread work for the time quantum
        pthread_mutex_unlock(&mutex);
        usleep(2000000);
        //check if the task is finished
        
    } 
    pthread_join(tid1, NULL);
    pthread_mutex_destroy(&mutex);
    return 0;  
}