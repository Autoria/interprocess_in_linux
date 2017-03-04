
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <stddef.h>
int listener(char *name)
{
	int listen_fd;
	struct sockaddr_un un;
	int rval, len;
	
	//create socket, bid to local
	if ((listen_fd = socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		perror("listen socket error");
		return -1;
	}
	unlink(name); //in case "name" already exists
	
	//fill in socket address structure
	un.sun_family = AF_UNIX;
	strncpy(un.sun_path, name, sizeof(un.sun_path)-1);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
	
	//bind the sockfd and address
	if ((bind(listen_fd, (struct sockaddr *)&un, len))<0) {
		rval = -2;
		goto errout;
	}
	
	//socket() craete fd default to initiative, use listen() trans 2 REAL listenfd.
	if (listen(listen_fd, 10) < 0) {
		rval = -3;
		goto errout;
	}
	
	return listen_fd;
	
errout:
	close(listen_fd);
	return rval;
}

int accept_client(int listen_fd)
{
	int connfd;
	struct sockaddr_un client_addr;
	socklen_t client_len = sizeof(client_addr);
	
	connfd = accept(listen_fd, (struct sockaddr*)&client_addr, &client_len);
	if(connfd < -1)
	{
		return -1;
	}
	
	return connfd;
}


int connect_server(const char *name)
{
	int client_fd;
	struct sockaddr_un server_addr;
	int rval, len;
	
	//create client socket
	if((client_fd =  socket(AF_UNIX, SOCK_STREAM, 0)) < 0){
		return -1;
	}
	
	//fill in socket address structure
	server_addr.sun_family = AF_UNIX;
	strncpy(server_addr.sun_path, name, sizeof(server_addr.sun_path)-1);
	len = offsetof(struct sockaddr_un, sun_path) + strlen(name);
	
	//connect to server
	if ((connect(client_fd, (struct sockaddr *)&server_addr, len))<0) {
		rval = -2;
		goto errout;
	}
	
	return client_fd;
	
errout:
	close(client_fd);
	return rval;
}

#define PATHNAME "/tmp/socket.domain"

int main()
{
	pid_t pid;
	
	pid = fork();
	if (pid < 0)
	{
		perror("fork error");
	}
	else if (pid != 0)
	{
		int listen_fd, connfd;
		char buf[128];
		int ret;
		
		printf("parent pid %d\n", getpid());
		listen_fd = listener(PATHNAME);
		connfd = accept_client(listen_fd);
		ret = read(connfd, buf, 124);
		
		if (ret > 0) {
			buf[ret] = '\0';
			printf("%d, %s\n", getpid(), buf);
		}
	}
	else
	{
		int client_fd;
		
		printf("child pid %d\n", getpid());
		if ((client_fd = connect_server(PATHNAME)) > 0)
		{
			write(client_fd, "i am child", strlen("i am child")+1);
		}
	}
	
	return 0;
}
