#include<stdio.h>
#include<semaphore.h>
#include<stdlib.h>
//这是一个第一类读写者问题，读者优先，可能导致饥饿 
int sem_post(sem_t* sem);
int sem_wait(sem_t* sem);
typedef struct sbuf_t{
	int *buf;
	int n;
	int front; //buf[(front+1)%n] is first item.
	int rear; //buf[rear%n] is last item.
	sem_t mutex;
	sem_t slots; //available slots.
	sem_t items; //available items.
} sbuf_t;

void P(sem_t *sem)
{
	if (sem_wait(sem) < 0)
	perror("P error\n");
}

void V(sem_t *sem)
{
	if (sem_post(sem) < 0)
	perror("V error\n");
}

int readcnt;
sem_t mutex, w;

void reader(void)
{
	while (1) 
	{
		P(&mutex);
		readcnt++;
		if (readcnt == 1)
		    P(&w);
        V(&mutex);
        //Critical section
        //Reading happens
        P(&mutex);
        readcnt--;
        if (readcnt == 0)
            V(&w);
        V(&mutex); 
	}
}

void writer(void)
{
	while (1)
	{
	    P(&w);
	    //Critical section
	    //Writing happens
	    V(&w);
	}
}

int main()
{
	readcnt = 0;
	sem_init(&mutex, 1);
	sem_init(&w, 1);
	
	return 0; 
}
