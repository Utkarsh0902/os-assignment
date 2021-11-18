#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
  
//clock_t start1, end1,start2, end2, start3, end3;
unsigned long long sum =0;
unsigned long long sum2 =0;  

//q =4;
//times = [6,8,12];

void *func1(void *arg){
    
    int i,j;
    int *n1 = (int*)arg;

     for(i=1;i<=*n1;i++) 
    {
        sum += (unsigned long long)i ;
    } 

    printf("C1 sum: %llu \n",sum);
    return NULL;

}  



void *func2(void *arg){
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
    return NULL;

}

void *func3(void *arg){
    
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
        sum2 += (unsigned long long)numberArray[i];

    }
    printf("C3 sum: %lld \n",sum2);
    return NULL;

}     






// Driver code
int main(int argc,char *argv[])
{

    
    // double cpu_time_used1,cpu_time_used2,cpu_time_used3;

    // start1 = clock();
    // start2 = clock();
    // start3 = clock();

    int pid, pid1, pid2;
    pthread_t tid1 ,tid2,tid3;
    int data1,data2,data3,errcode,i;
    
    
    
    data1 = atoi (argv[1]);
    data2 = atoi (argv[2]);
    data3 = atoi (argv[3]);
    printf("%d %d %d \n",data1,data2,data3);

    if((data1<25 || data1>1000000 || data2<25 || data2>1000000 || data3<25 || data3>1000000))
    {
        printf("Invalid input \n");
        exit(0);
    }
    
    pid = fork();

   
    
  
    if (pid == 0) {
  
    // child process 

    pthread_create(&tid1,NULL,func1,(void*)&data1);
    // end1 = clock(); 
    
    
    }
  
    else{
        pid1 = fork();
        if (pid1 == 0) {

            pthread_create(&tid2,NULL,func2,(void*)&data2);
            // end2 = clock();

            

        }
    
        else {
            pid2 = fork();
            if (pid2 == 0) {

                pthread_create(&tid3,NULL,func3,(void*)&data3);
                // end3 = clock();

                
                
            }
  
            // If value returned from fork()
            // in not zero and >0 that means
            // this is parent process.
            else {
                // This is asked to be printed at last
                // hence made to sleep for 3 seconds.
                sleep(3);
                printf("parent --> pid = %d\n", getpid());
            }
        }
    }



    
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);
    pthread_join(tid1,NULL);

    
    // cpu_time_used1 = ((double) (end1 - start1));
    // printf("Time used C1 : %lf",cpu_time_used1);

    // cpu_time_used2 = ((double) (end2 - start2));
    // printf("Time used C2 : %lf",cpu_time_used2);
    
    // cpu_time_used3 = ((double) (end3 - start3));
    // printf("Time used C3 : %lf",cpu_time_used3);


  
    return 0;
}
