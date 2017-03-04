#include<stdio.h>
#include<semaphore.h>
#include<stdlib.h>

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

void sbuf_init(sbuf_t *sp, int n)
{
	sp->buf = calloc(n, sizeof(int));
	sp->n = n;
	sp->front = sp->rear = 0;
	sem_init(&sp->mutex, 0, 1);
	sem_init(&sp->slots, 0, n);
	sem_init(&sp->items, 0, 0);
}

void sbuf_deinit(sbuf_t *sp)
{
	free(sp->buf);
}

void sbuf_insert(sbuf_t *sp, int item)
{
	P(&sp->slots);
	P(&sp->mutex);
	sp->buf[(++sp->rear)%(sp->n)] = item;
	V(&sp->mutex);
	V(&sp->items);
}

int sbuf_remove(sbuf_t *sp)
{
	int item;
	P(&sp->items);
	P(&sp->mutex);
	item = sp->buf[(++sp->front)%(sp->n)];
	V(&sp->mutex);
	V(&sp->slots);
	
	return item;
}


int main()
{
	sbuf_t *sp = malloc(sizeof(sbuf_t));
	sbuf_init(sp, 10);
	sbuf_insert(sp, 666);
	int item = sbuf_remove(sp);
	printf("%d\n", item);
	
	sbuf_deinit(sp); 
	return 0; 
}
