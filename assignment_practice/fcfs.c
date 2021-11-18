#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
int main(int argc, char *argv[])
{
    int i=0,pipeToChild[2], pipeToParent[2];
    pid_t cpid;
    char buf, sleepTime[64]={0};
    if (argc != 2) {
      fprintf(stderr, "Usage: %s <sleepTime>\n", argv[0]);
      exit(EXIT_FAILURE);
    }
    // Create the two pipes
    if (pipe(pipeToChild) == -1 || pipe(pipeToParent) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    // Fork the child
    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    // --------------------------------------------------
    // This is the child
    if (cpid == 0) {  
        // Close the pipes the child shouldn't use 
        close(pipeToParent[0]);
        close(pipeToChild[1]);
        // Setup the input and output of child    
        dup2(pipeToChild[0],0);
        dup2(pipeToParent[1],1);
        // Start the sleeper program
        execlp("./sleeper.out", "sleeper",NULL);    // <------------  Sleeper program started
        printf("Error:  Should never get here!!\n");
    } 
    // --------------------------------------------------
    // This is the parent
    else {           
      // Close the pipes the parent shouldn't use
      close(pipeToChild[0]);          
      close(pipeToParent[1]);
      sprintf(sleepTime,"%s\n",argv[1]);
      // Write the amount of time to sleep to the child
      write(pipeToChild[1], sleepTime, strlen(sleepTime));
      // Clear buff
      memset(sleepTime,0,sizeof(sleepTime));
      // The child should send back something
      while ( i< 64 && read(pipeToParent[0],&buf,1)){
        sleepTime[i++]=buf;
      }
      close(pipeToChild[1]);         
      close(pipeToParent[0]);
      // Wait for child
      wait(NULL);            
      printf("Child message:  '%s'\n",sleepTime);
      exit(EXIT_SUCCESS);
    }
}