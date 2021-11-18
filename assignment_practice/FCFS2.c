#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/time.h>

pthread_mutex_t shm_mutex;
pthread_mutex_t c1_mutex, c2_mutex, c3_mutex;

long parent_start;
struct timeval timecheck;

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

    long start, end;
    struct timeval timecheck;
    pthread_mutex_lock(&c1_mutex);
    
    gettimeofday(&timecheck, NULL);
    start = (long)timecheck.tv_usec;
    printf("C1 starts at t = %ldus \n",start-parent_start);
    


    unsigned long long sum =0;
    int *n1 = (int*)arg;

     for(int i=1;i<=*n1;i++) 
    {
        sum += (unsigned long long)i ;
    } 

    printf("C1 sum: %llu \n",sum);

    gettimeofday(&timecheck, NULL);
    end = (long)timecheck.tv_usec;
    printf("C1 ends at t = %ldus \n",end-parent_start);

    pthread_mutex_unlock(&c1_mutex);
    return NULL;
}


void* func2(void *arg){

    long start, end;
    struct timeval timecheck;
    pthread_mutex_lock(&c2_mutex);

    gettimeofday(&timecheck, NULL);
    start = (long)timecheck.tv_usec;
    printf("C2 starts at t = %ldus \n",start-parent_start);



    FILE *myFile;
    myFile = fopen("somenumbers.txt", "r");
     //read file into array
    int *numberArray =(int*) malloc(sizeof(int)*1000000);
    int i;
    int *n2 = (int*)arg;
    for (i = 0; i < *n2; i++)
    {
        fscanf(myFile, "%d", &numberArray[i]);
        if(numberArray[i]<1 || numberArray[i]>1000000){
            perror("Number out of bounds.\nExiting..\n");
            exit(1);
        }
    }
    for (i = 0; i < *n2; i++)
    {
        printf("C2:%d\n", numberArray[i]);
    }

    gettimeofday(&timecheck, NULL);
    end = (long)timecheck.tv_usec;
    printf("C2 ends at t = %ldus \n",end-parent_start);

    pthread_mutex_unlock(&c2_mutex);
    return NULL;
}


void* func3(void *arg){
    long start, end;
    struct timeval timecheck;
    pthread_mutex_lock(&c3_mutex);

    gettimeofday(&timecheck, NULL);
    start = (long)timecheck.tv_usec;
    printf("C3 starts at t = %ldus \n",start-parent_start);
    

    unsigned long long sum =0;  
    FILE *myFile;
    myFile = fopen("somenumbers2.txt", "r");
     //read file into array
    int *numberArray =(int*) malloc(sizeof(int)*1000000);
    int i;
    int *n3 = (int*)arg;
    for (i = 0; i < *n3; i++)
    {
        fscanf(myFile, "%d", &numberArray[i]);
        if(numberArray[i]<1 || numberArray[i]>1000000){
            perror("Number out of bounds.\nExiting..\n");
            exit(1);
        }
        sum += (unsigned long long)numberArray[i];

    }
    printf("C3 sum: %lld \n",sum);

    gettimeofday(&timecheck, NULL);
    end = (long)timecheck.tv_usec;
    printf("C3 ends at t = %ldus \n",end-parent_start);

    pthread_mutex_unlock(&c3_mutex);
    return NULL;    
}


int main(int argc,char *argv[]){
    void *shmPtr;
    char buf[10];
    int shmid;
    pid_t c1 = -1 ,c2 = -1 ,c3 = -1;
    pthread_t tid1 ,tid2,tid3;
    int data1,data2,data3;

    data1 = atoi (argv[1]);
    data2 = atoi (argv[2]);
    data3 = atoi (argv[3]);

    
    
    gettimeofday(&timecheck, NULL);
    parent_start = (long)timecheck.tv_usec;

    if((data1<25 || data1>1000000 || data2<25 || data2>1000000 || data3<25 || data3>1000000))
    {
        perror("Invalid input.\nExiting..\n");
        exit(1);
    }

    c1 = fork();
    checkError(c1, 'f');

    if(c1!=0 && c1!= -1){
        c2 = fork();
        checkError(c2, 'f');
    }
    if(c1!=0 && c1!=-1 && c2!=0 && c2!=-1){
        c3 = fork();
        checkError(c3, 'f');
    }

    /* #### PARENT PROCESS #### */
    if(c1!= 0 && c1!= -1 && c2!= 0 && c2!=-1 && c3!= 0 && c3!=-1){
        
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

        // SCHEDULE

        // C1 first
        strcpy((char*)shmPtr, "c1_start");
        // busy wait for c1 to be finished
        while(strcmp(buf,"c1_done")!=0){
            // 2us polling
            usleep(2);
            strcpy(buf, (char*)shmPtr);
        }

        printf("Parent: c1 is done.\n");

        // C2 NEXT
        strcpy((char*)shmPtr, "c2_start");
        // busy wait for c2 to be finished
        while(strcmp(buf,"c2_done")!=0){
            // 2us polling
            usleep(2);
            strcpy(buf, (char*)shmPtr);
        }

        printf("Parent: c2 is done.\n");

        // C3 NEXT
        strcpy((char*)shmPtr, "c3_start");
        // busy wait for c3 to be finished
        while(strcmp(buf,"c3_done")!=0){
            // 2us polling
            usleep(2);
            strcpy(buf, (char*)shmPtr);
        }

        printf("Parent: c3 is done.\n");

        wait(NULL);
        wait(NULL);
        wait(NULL);
    }

    /* ### C1 Process ### */
    if(c1==0){

        pthread_mutex_init(&c1_mutex, NULL);
        pthread_mutex_lock(&c1_mutex);
        pthread_create(&tid1,NULL,func1,(void*)&data1);

        //  Accessing the shared memory
        pthread_mutex_lock(&shm_mutex);
            if((shmid = shmget(2041, 32, 0)) == -1){
                perror("Error in shmget() in c1\n");
                exit(1);
            }
            shmPtr = shmat(shmid, 0, 0);
            if(shmPtr == (char*)-1){
                perror("Error in shmat() in child\n");
                exit(2);
            }
        pthread_mutex_unlock(&shm_mutex);


        // POLLING
        strcpy(buf, (char*)shmPtr);
        while(strcmp(buf, "c1_start")!=0){
            // 2us polling
            usleep(2);
            strcpy(buf, (char*)shmPtr);
        }

        // Signal to start work has been recieved
        pthread_mutex_unlock(&c1_mutex);
        pthread_join(tid1, NULL);

        // now the thread is done working
        strcpy((char*)shmPtr, "c1_done");
        exit(0); 
    }

    // /* ### C2 Process ### */
    if(c2==0){
        pthread_mutex_init(&c2_mutex, NULL);
        pthread_mutex_lock(&c2_mutex);
        pthread_create(&tid2,NULL,func2,(void*)&data2);

        //  Accessing the shared memory
        pthread_mutex_lock(&shm_mutex);
            if((shmid = shmget(2041, 32, 0)) == -1){
                perror("Error in shmget() in c2\n");
                exit(1);
            }
            shmPtr = shmat(shmid, 0, 0);
            if(shmPtr == (char*)-1){
                perror("Error in shmat() in child\n");
                exit(2);
            }
        pthread_mutex_unlock(&shm_mutex);


        // POLLING
        strcpy(buf, (char*)shmPtr);
        while(strcmp(buf, "c2_start")!=0){
            // 2us polling
            usleep(2);
            strcpy(buf, (char*)shmPtr);
        }

        // Signal to start work has been recieved
        pthread_mutex_unlock(&c2_mutex);
        pthread_join(tid2, NULL);

        // now the thread is done working
        strcpy((char*)shmPtr, "c2_done");
        exit(0); 
    }

    /* ### C3 Process ### */
    if(c3==0){
        pthread_mutex_init(&c3_mutex, NULL);
        pthread_mutex_lock(&c3_mutex);
        pthread_create(&tid3,NULL,func3,(void*)&data3);

        //  Accessing the shared memory
        pthread_mutex_lock(&shm_mutex);
            if((shmid = shmget(2041, 32, 0)) == -1){
                perror("Error in shmget() in c3\n");
                exit(1);
            }
            shmPtr = shmat(shmid, 0, 0);
            if(shmPtr == (char*)-1){
                perror("Error in shmat() in child\n");
                exit(2);
            }
        pthread_mutex_unlock(&shm_mutex);


        // POLLING
        strcpy(buf, (char*)shmPtr);
        while(strcmp(buf, "c3_start")!=0){
            // 2us polling
            usleep(2);
            strcpy(buf, (char*)shmPtr);
        }

        // Signal to start work has been recieved
        pthread_mutex_unlock(&c3_mutex);
        pthread_join(tid3, NULL);

        // now the thread is done working
        strcpy((char*)shmPtr, "c3_done");
        exit(0); 
    }

    return 0;
}