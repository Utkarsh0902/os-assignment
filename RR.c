#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>
#include <sys/time.h>

pthread_mutex_t shm_mutex;
pthread_mutex_t time_mutex;
pthread_cond_t child_cond[3];

int time_quantum;// Minimum: 10u seconds (for output to work fine)

unsigned long long  parent_start;
struct timeval timecheck;

unsigned long long start[3], end[3], waiting[3];


struct task{
    pid_t pid; // ID of the child process
    pthread_t tid; // ID of the worker thread
    pthread_mutex_t task_mutex; // Mutex used in the worker thread
    bool isComplete; // To check if the task is completed.
};

struct task child[3];

struct queue{
    int q[3];
    int rear;
};
struct queue ready_queue;

void enqueue(struct queue *rq, int x){
    if(rq->rear<2){
        rq->rear+=1;
        rq->q[rq->rear] = x; 
    }
    else{
        perror("Queue size exceeded.\n");
    }
}

int deque(struct queue *rq){
    int x;
    if(rq->rear == -1){
        perror("Empty queue.\n");
    }
    else{
        x = rq->q[0];
        for(int i =0; i<rq->rear; i++){
            rq->q[i] = rq->q[i+1];
        }
        rq->rear-=1;
    }
    return x;
}

bool isEmpty(struct queue *rq){
    if(rq->rear == -1){
        return true;
    }
    return false;
}
 

void checkError(int id, char proc){
    if(id == -1){
        switch(proc){
            case 'p':
                perror("Error while creating pipe.");
                exit(EXIT_FAILURE);
                break;
            case 'f':
                perror("Error while forking.");
                exit(EXIT_FAILURE);
                break;
        }
    }
}

void* func1(void *arg){
    struct timeval timecheck;
    pthread_mutex_lock(&child[0].task_mutex);
    gettimeofday(&timecheck, NULL);
    start[0] = (unsigned long long )timecheck.tv_usec- parent_start;
    unsigned long long sum =0;
    int *n1 = (int*)arg;
    pthread_cond_wait(&child_cond[0], &child[0].task_mutex);

    for(int i=1;i<=*n1;i++){
        pthread_cond_wait(&child_cond[0], &child[0].task_mutex);
        sum += (unsigned long long)i;
    } 

    pthread_cond_wait(&child_cond[0], &child[0].task_mutex);
    printf("C1 sum: %llu \n",sum);
    gettimeofday(&timecheck, NULL);
    end[0] = (unsigned long long)timecheck.tv_usec - parent_start;
    child[0].isComplete = true;
    pthread_mutex_unlock(&child[0].task_mutex);
    return NULL;
}


void* func2(void *arg){

    struct timeval timecheck;
    pthread_mutex_lock(&child[1].task_mutex);

    gettimeofday(&timecheck, NULL);
    start[1] = (unsigned long long )timecheck.tv_usec - parent_start;
    
    FILE *myFile;
    myFile = fopen("somenumbers.txt", "r");
    
    
    pthread_cond_wait(&child_cond[1], &child[1].task_mutex);

    //read file into array
    int *numberArray =(int*) malloc(sizeof(int)*1000000), *n2 = (int*)arg;
    
    pthread_cond_wait(&child_cond[1], &child[1].task_mutex);
    
    for (int i = 0; i < *n2; i++)
    {
        pthread_cond_wait(&child_cond[1], &child[1].task_mutex);
            fscanf(myFile, "%d", &numberArray[i]);
        
        pthread_cond_wait(&child_cond[1], &child[1].task_mutex);
        if(numberArray[i]<1 || numberArray[i]>1000000){
            perror("Number out of bounds.\nExiting..\n");
            exit(1);
        }
        
    }
    for (int i = 0; i < *n2; i++){
        pthread_cond_wait(&child_cond[1], &child[1].task_mutex);
        printf("C2:%d\n", numberArray[i]);
    }

    pthread_cond_wait(&child_cond[1], &child[1].task_mutex);

    gettimeofday(&timecheck, NULL);
    end[1] = (unsigned long long)timecheck.tv_usec - parent_start;

    child[1].isComplete = true;
    pthread_mutex_unlock(&child[1].task_mutex);
    return NULL;
}


void* func3(void *arg){

    struct timeval timecheck;
    pthread_mutex_lock(&child[2].task_mutex);

    gettimeofday(&timecheck, NULL);
    start[2] = (unsigned long long )timecheck.tv_usec - parent_start;
    unsigned long long sum =0;  
    
    pthread_cond_wait(&child_cond[2], &child[2].task_mutex);
    
    FILE *myFile;
    myFile = fopen("somenumbers2.txt", "r");

    pthread_cond_wait(&child_cond[2], &child[2].task_mutex);

    //read file into array
    int *numberArray =(int*) malloc(sizeof(int)*1000000), *n3 = (int*)arg;
     
    for (int i = 0; i < *n3; i++)
    {
        pthread_cond_wait(&child_cond[2], &child[2].task_mutex);

        fscanf(myFile, "%d", &numberArray[i]);

        pthread_cond_wait(&child_cond[2], &child[2].task_mutex);

        if(numberArray[i]<1 || numberArray[i]>1000000){
            perror("Number out of bounds.\nExiting..\n");
            exit(1);
        }

        pthread_cond_wait(&child_cond[2], &child[2].task_mutex);
        sum += (unsigned long long)numberArray[i];

    }
    pthread_cond_wait(&child_cond[2], &child[2].task_mutex);
    
    printf("C3 sum: %lld \n",sum);

    gettimeofday(&timecheck, NULL);
    end[2] = (unsigned long long)timecheck.tv_usec - parent_start;

    child[2].isComplete = true;
    pthread_mutex_unlock(&child[2].task_mutex);
    return NULL;    
}

void monitor(int it, int shmid, void* shmPtr, char buf[10], char start_text[10]){
    pthread_mutex_init(&child[it].task_mutex, NULL);
    pthread_mutex_lock(&child[it].task_mutex);


    struct timeval timecheck;
    


    //  Accessing the shared memory
    pthread_mutex_lock(&shm_mutex);
        if((shmid = shmget(2041, 32, 0)) == -1){
            perror("Error in shmget() in child[it].pid\n");
            exit(1);
        }
        shmPtr = shmat(shmid, 0, 0);
        if(shmPtr == (char*)-1){
            perror("Error in shmat() in child\n");
            exit(2);
        }
    pthread_mutex_unlock(&shm_mutex);


    // POLLING
    
    while(child[it].isComplete == false){

        gettimeofday(&timecheck, NULL);
        unsigned long long wait_temp = (unsigned long long )timecheck.tv_sec;

        strcpy(buf, (char*)shmPtr);
        while(strcmp(buf, start_text)!=0){
            // 2 us polling
            usleep(2);
            strcpy(buf, (char*)shmPtr);
        }
        // Signal to start work has been recieved
        gettimeofday(&timecheck, NULL);
        waiting[it]+= (unsigned long long )timecheck.tv_sec- wait_temp - parent_start*1000000;

        pthread_mutex_lock(&time_mutex);
        pthread_mutex_unlock(&child[it].task_mutex);
        pthread_cond_signal(&child_cond[it]);

        // wait for the time quantum to end
        strcpy(buf, (char*)shmPtr);
        while(strcmp(buf, "stop")!=0){
            // 2 us polling
            usleep(2);
            strcpy(buf, (char*)shmPtr);
                pthread_cond_signal(&child_cond[it]); // this signal means that the thread can still work
        }
        pthread_mutex_lock(&child[it].task_mutex);
        pthread_mutex_unlock(&time_mutex);
    }
    strcpy((char*)shmPtr, "complete");

    printf("C%d start time:%llu\n", it+1, start[it]);
    printf("C%d end time:%llu\n",it+1,  end[it]);
    printf("C%d wait time:%llu\n", it+1, waiting[it]);
    exit(0); 
}

int main(int argc,char *argv[]){
    void *shmPtr;
    char buf[10];
    int shmid;
    child[0].pid = -1; child[1].pid = -1; child[2].pid = -1;
    child[0].isComplete = false; child[1].isComplete = false; child[2].isComplete = false;

    ready_queue.rear = -1;
    int data1,data2,data3;

    data1 = atoi (argv[1]);
    data2 = atoi (argv[2]);
    data3 = atoi (argv[3]);
    time_quantum = atoi (argv[4]);

    gettimeofday(&timecheck, NULL);
    parent_start = (unsigned long long)timecheck.tv_usec;
    waiting[0] = 0;
    waiting[1] = 0;
    waiting[2] = 0;

    if((data1<25 || data1>1000000 || data2<25 || data2>1000000 || data3<25 || data3>1000000))
    {
        perror("Invalid input.\nExiting..\n");
        exit(1);
    }

    child[0].pid = fork();
    checkError(child[0].pid, 'f');

    if(child[0].pid!=0 && child[0].pid!= -1){
        child[1].pid = fork();
        checkError(child[1].pid, 'f');
    }
    if(child[0].pid!=0 && child[0].pid!=-1 && child[1].pid!=0 && child[1].pid!=-1){
        child[2].pid = fork();
        checkError(child[2].pid, 'f');
    }

    pthread_cond_init(&child_cond[0], NULL);
    pthread_cond_init(&child_cond[1], NULL);
    pthread_cond_init(&child_cond[2], NULL);

    /* #### PARENT PROCESS #### */
    if(child[0].pid!= 0 && child[0].pid!= -1 && child[1].pid!= 0 && child[1].pid!=-1 && child[2].pid!= 0 && child[2].pid!=-1){
        
        pthread_mutex_init(&time_mutex, NULL);
        pthread_mutex_lock(&time_mutex);

        // Setting up the shared memory
        pthread_mutex_init(&shm_mutex, NULL);
        pthread_mutex_lock(&shm_mutex);
            if((shmid = shmget(2041,32,0644 | IPC_CREAT)) == -1){
                perror("Error in shmget() in parent\n");
                exit(3);
            }
            //attach to the space
            shmPtr = shmat(shmid, 0, 0);
            if(shmPtr == (char*)-1){
                perror("Error in shmat() in parent\n");
                exit(4);
            }
        pthread_mutex_unlock(&shm_mutex);

        enqueue(&ready_queue, 0);
        enqueue(&ready_queue, 1);
        enqueue(&ready_queue, 2);

        // SCHEDULE
        printf("Schedule is starting \n");
        while(!isEmpty(&ready_queue)){
            pid_t pid_idx = deque(&ready_queue);
            //printf("parent: pid_idx=%d\n", pid_idx+1);
            switch (pid_idx)
            {
                case 0:
                    strcpy((char*)shmPtr, "c1_start");
                    break;
                
                case 1:
                    strcpy((char*)shmPtr, "c2_start");
                    break;
                
                case 2:
                    strcpy((char*)shmPtr, "c3_start");
                    break;
                default:
                    perror("Wrong pid");
                    exit(1);
                    break;
            }
            pthread_mutex_unlock(&time_mutex);
            usleep(time_quantum);
            strcpy((char*)shmPtr, "stop");
            usleep(1000);
            pthread_mutex_lock(&time_mutex);
            if(strcmp((char*)shmPtr,"complete")!= 0){
                enqueue(&ready_queue, pid_idx);
            }

        }
        printf("Schedule complete.\n");
        wait(NULL);
        wait(NULL);
        wait(NULL);
    }

    /* ### C1 Process ### */
    if(child[0].pid==0){
        pthread_create(&child[0].tid,NULL,func1,(void*)&data1);
        monitor(0,shmid, shmPtr, buf, "c1_start");
        printf("C1 complete\n");
        exit(0);
    }

    /* ### C2 Process ### */
    if(child[1].pid==0){
        pthread_create(&child[1].tid,NULL,func2,(void*)&data2);
        monitor(1,shmid, shmPtr, buf, "c2_start");
        printf("C2 complete\n");
        exit(0); 
    }

    /* ### C3 Process ### */
    if(child[2].pid==0){
        pthread_create(&child[2].tid,NULL,func3,(void*)&data3);
        monitor(2,shmid, shmPtr, buf, "c3_start");
        printf("C3 complete\n");
        exit(0); 
    }
    return 0;
}