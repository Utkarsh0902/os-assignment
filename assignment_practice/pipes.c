#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<unistd.h>
 
int main(){
// PIPES CREATIONS
    int pfds[2]; // pipe file descriptors
    char buff[30]; // writing
    
    if(pipe(pfds) == -1){
        perror("Error in pipe()\n");
        exit(1);
    }
    printf("Writing to file descriptor %d\n", pfds[1]);
    write(pfds[1], "test", 5);
    read(pfds[0], buff, 5);
    printf("Read from pipe : %s \n", buff);
    return 0;
}