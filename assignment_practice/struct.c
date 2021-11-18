#include<stdio.h>
#include<stdlib.h>

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
int main(){
    struct queue q;
    q.rear =-1;
    enqueue(&q, 10);
    enqueue(&q, 3);
    enqueue(&q, 2);

    printf("%d\n", deque(&q));
    printf("%d\n", deque(&q));
    enqueue(&q,3);
    printf("%d\n", deque(&q));
    printf("%d\n", deque(&q));
    printf("%d\n", deque(&q));
    return 0;
}