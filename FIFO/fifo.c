#include<unistd.h>
#include<stdlib.h>
#include<stdio.h>
#include<pthread.h>
#include<fcntl.h>
#include <sys/stat.h>

#define FIFO "test_fifo"

void *read_fifo(void* dummy)
{
	int fd = open(FIFO, O_RDONLY);

	char read_buf[1024];
	read(fd, read_buf, 1024);
	
	write(STDOUT_FILENO, read_buf, 1024);
	close(fd);
	return NULL;
}

void *write_fifo(void* dummy)
{
	int fd = open(FIFO, O_WRONLY);
	write(fd, "test1 running now.\n",19);
	close(fd);
	return NULL;
}

void test1()
{
	pthread_t tid1;
	pthread_t tid2;
	
	mkfifo(FIFO, S_IRUSR | S_IWUSR| S_IRGRP|S_IWGRP);
	pthread_create(&tid1, NULL, read_fifo, NULL);
	pthread_create(&tid2, NULL, write_fifo, NULL);
	
    pthread_join(tid1,NULL);
    pthread_join(tid2,NULL);
	
} 

int main()
{
	test1();
}
