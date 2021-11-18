#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

int main(){
  int sleepTime;
  if ( scanf("%d",&sleepTime) == 1 ){
    sleep(sleepTime);
    printf("slept %d",sleepTime);
  } else {
    printf("Error\n");
  }
  return 0;
}