#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

#define NUM_THREADS 1
unsigned long long sum =0;
unsigned long long sum2 =0;

void *func1(void *arg){
    
    int i,j;

    int *n1 = (int*)arg;

     for(i=1;i<=*n1;i++) 
    {
        sum += (unsigned long long)i ;
    } 
    
    printf("Sum of n1 numbers in C1 %llu \n",sum);
    
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
    }

    for (i = 0; i < *n2; i++)
    {
        printf("Number is: %d\n", numberArray[i]);
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
        sum2 += (unsigned long long)numberArray[i];

    }

    printf("Sum for process C3 %lld \n",sum2);
    return NULL;

    }   


int main(int argc,char *argv[])
{
    pthread_t tid1 ,tid2,tid3;
    int data1,data2,data3,errcode,i;
    
    
    data1 = atoi (argv[1]);
    data2 = atoi (argv[2]);
    data3 = atoi (argv[3]);

    printf("%d %d %d \n",data1,data2,data3);

    pthread_create(&tid1,NULL,func1,(void*)&data1);
    
    pthread_create(&tid2,NULL,func2,(void*)&data2);

    pthread_create(&tid3,NULL,func3,(void*)&data3);

    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
    pthread_join(tid3,NULL);
    
    
    
    return 0;
}
