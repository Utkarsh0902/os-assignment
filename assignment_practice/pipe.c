#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <sys/wait.h>
#include <pthread.h>
#include <sys/types.h>

void main()
{   
    int childProcess;
    int returnValue1;
    int returnValue2;
    int c1res;
    int c3res;
    pid_t pid;

    int pfds[2];
    char buf[30];

    printf("Please enter the process number\n");
    scanf("%d", &childProcess);

    if (childProcess == 2){
        pipe(pfds);
        pid = fork();
        if(pid==0){
            //C2
            close(pfds[0]);
            //C2 prints all the numbers
            write(pfds[1], "Done Printing", 14);
            exit(0);
        }
        else if (pid>0)
        {
            wait(NULL);
            close(pfds[1]);
            read(pfds[0], buf, 14);
            printf("%s", buf);
        }
        else
        {
            perror("Error\n");
        }
    }
    else if (childProcess == 1)
    {   
        printf("Please enter the return value from the Process C1: ");
        scanf("%d", &returnValue1);
        pipe(pfds);
        pid = fork();
        if (pid ==0){
            //C1 
            close(pfds[0]);
            //C1 
            write(pfds[1], &returnValue1, sizeof(returnValue1));
            exit(0);
        }
        else if (pid>0)
        {
            wait(NULL);
            close(pfds[1]);
            read(pfds[0], &c1res, sizeof(c1res));
            printf("%d\n", c1res);
        }
        else
        {
            perror("Error\n");
        }
    }
    else if (childProcess == 3)
    {
        printf("Please enter the return value from the Process C2: ");
        scanf ("%d", &returnValue2);
        pipe(pfds);
        pid = fork();
        if (pid ==0){
            //C1 
            close(pfds[0]);
            //C1 
            write(pfds[1], &returnValue2, sizeof(returnValue2));
            exit(0);
        }
        else if (pid>0)
        {
            wait(NULL);
            close(pfds[1]);
            read(pfds[0], &c3res, sizeof(c3res));
            printf("%d\n", c3res);
        }
        else
        {
            perror("Error\n");
        }
    }
    else
    {
        printf("Process doesnt exist\n");
    }
}


