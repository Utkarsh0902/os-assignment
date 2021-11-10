#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

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
    while(1){
        printf("C1\n");
        sleep(1);
    }
    return NULL;
}
void* func2(void *arg){
    while(1){
        printf("C2\n");
        sleep(1);
    }
    return NULL;
}
void* func3(void *arg){
    while(1){
        printf("C3\n");
        sleep(1);
    }
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

    if(c1 != 0){
        // inside parent after c1 spawns
        c2 = fork();
        checkError(c2, 'f');

        if(c2 != 0){
            // inside parent after c2 spawns
            c3 = fork();
            checkError(c3, 'f');

            if(c3 != 0){
                // inside parent after c3 spawns
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
                while(strcmp(buf,"END")){
                    printf("Parent func. \nEnter scheduled task:\n");
                    scanf("%s", buf);
                    strcpy(shmPtr, buf);
                }
                wait(NULL);
                wait(NULL);
                wait(NULL);
            }
            else{
                // inside c3
                sleep(2);
                // get the id of the shared block
                if((shmid = shmget(2041, 32, 0)) == -1){
                    perror("Error in shmget() in C3\n");
                    exit(1);
                }
                // attach to the shared block
                shmPtr = shmat(shmid, 0, 0);
                if(shmPtr == (char*)-1){
                    perror("Error in shmat() in child\n");
                    exit(2);
                }
                
                // keep polling till END is recieved
                while(strcmp(buf,"END")){
                    while(strcmp((char*)shmPtr, "C3")){
                        //sleep for 20 milliseconds = 20k microseconds
                        usleep(20000);
                    }
                    strcpy(buf, (char*)shmPtr);
                    printf("Perform tasks in C3.\n");
                    sleep(5);
                }
            }
        }
        else{
            // inside c2
            sleep(2);
            // get the id of the shared block
            if((shmid = shmget(2041, 32, 0)) == -1){
                perror("Error in shmget() in C2\n");
                exit(1);
            }
            // attach to the shared block
            shmPtr = shmat(shmid, 0, 0);
            if(shmPtr == (char*)-1){
                perror("Error in shmat() in child\n");
                exit(2);
            }
            
            // keep polling till END is recieved
            while(strcmp(buf,"END")){
                while(strcmp((char*)shmPtr, "C2")){
                    //sleep for 20 milliseconds = 20k microseconds
                    usleep(20000);
                }
                strcpy(buf, (char*)shmPtr);
                printf("Perform tasks in C2.\n");
                sleep(5);
            }
        }
    }
    else{
        // inside c1
        sleep(2);
        // get the id of the shared block
        if((shmid = shmget(2041, 32, 0)) == -1){
            perror("Error in shmget() in C1\n");
            exit(1);
        }
        // attach to the shared block
        shmPtr = shmat(shmid, 0, 0);
        if(shmPtr == (char*)-1){
            perror("Error in shmat() in child\n");
            exit(2);
        }

        // keep polling till END is recieved
        while(strcmp(buf,"END")){
            while(strcmp((char*)shmPtr, "C1")){
                //sleep for 20 milliseconds = 20k microseconds
                usleep(20000);
            }
            strcpy(buf, (char*)shmPtr);
            printf("Perform tasks in C1.\n");
            sleep(5);
        } 
        
        //pthread_create(&tid1,NULL,func1,(void*)&data1);
    }
    return 0;
}