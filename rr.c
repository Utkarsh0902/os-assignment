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
    printf("c1\n");
    sleep(1);
    return NULL;
}
void* func2(void *arg){
    printf("c2\n");
    sleep(1);
    return NULL;
}
void* func3(void *arg){
    printf("c3\n");
    sleep(1);
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
        // Do all scheduling here
        printf("This is parent. PID: %d\n", getpid());
        // shared memory for communicating with the child processes
        // create the space
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

        // now write something to shmPtr
        strcpy(shmPtr, "");
        while(strcmp(buf,"end")){
            printf("Parent func. \nEnter scheduled task:\n");
            scanf("%s", buf);
            strcpy(shmPtr, buf);
        }
        printf("Out of while in parent.\n");
        wait(NULL);
        wait(NULL);
        wait(NULL);
    }

    /* ### C1 Process ### */
    if(c1==0){
        // inside c1
        sleep(2);
        // get the id of the shared block
        if((shmid = shmget(2041, 32, 0)) == -1){
            perror("Error in shmget() in c1\n");
            exit(1);
        }
        // attach to the shared block
        shmPtr = shmat(shmid, 0, 0);
        if(shmPtr == (char*)-1){
            perror("Error in shmat() in child\n");
            exit(2);
        }

        // keep polling till end is recieved
        strcpy(buf, (char*)shmPtr);
        while(strcmp(buf,"end")){
            strcpy(buf, (char*)shmPtr);
            while(strcmp(buf, "c1") && strcmp(buf, "end")){
                //sleep for 20 milliseconds = 20k microseconds
                strcpy(buf, (char*)shmPtr);
                usleep(20000);
            }
            printf("Perform tasks in c1.\n");
            printf("%s\n", buf);
            sleep(2);
        }
        printf("c1 ending\n");
        exit(0); 
        
        //pthread_create(&tid1,NULL,func1,(void*)&data1);
    }

    /* ### C2 Process ### */
    if(c2==0){
        // inside c2
        sleep(2);
        // get the id of the shared block
        if((shmid = shmget(2041, 32, 0)) == -1){
            perror("Error in shmget() in c2\n");
            exit(1);
        }
        // attach to the shared block
        shmPtr = shmat(shmid, 0, 0);
        if(shmPtr == (char*)-1){
            perror("Error in shmat() in child\n");
            exit(2);
        }
        
        // keep polling till end is recieved
        strcpy(buf, (char*)shmPtr);
        while(strcmp(buf,"end")){
            strcpy(buf, (char*)shmPtr);
            while(strcmp(buf, "c2") && strcmp(buf, "end")){
                //sleep for 20 milliseconds = 20k microseconds
                strcpy(buf, (char*)shmPtr);
                usleep(20000);
            }
            printf("Perform tasks in c2.\n");
            printf("%s\n", buf);
            sleep(2);
        }
        printf("c2 ending\n");
        exit(0);
    }

    /* ### C3 Process ### */
    if(c3==0){
        // inside c3
        sleep(2);
        // get the id of the shared block
        if((shmid = shmget(2041, 32, 0)) == -1){
            perror("Error in shmget() in c3\n");
            exit(1);
        }
        // attach to the shared block
        shmPtr = shmat(shmid, 0, 0);
        if(shmPtr == (char*)-1){
            perror("Error in shmat() in child\n");
            exit(2);
        }
        
        // keep polling till end is recieved
        strcpy(buf, (char*)shmPtr);
        while(strcmp(buf,"end")){
            strcpy(buf, (char*)shmPtr);
            while(strcmp(buf, "c3") && strcmp(buf, "end")){
                //sleep for 20 milliseconds = 20k microseconds
                strcpy(buf, (char*)shmPtr);
                usleep(20000);
            }
            printf("Perform tasks in c3.\n");
            printf("%s\n", buf);
            sleep(2);
        }
        printf("c3 ending\n");
        exit(0); 
    }

    return 0;
}