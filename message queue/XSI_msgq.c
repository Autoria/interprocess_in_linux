#include<sys/types.h>
#include<sys/msg.h>
#include<unistd.h>
#include<stdio.h>
void msg_stat(int, struct msqid_ds);
struct msgsbuf{
	int mtype;
	char mtext[1];
}msg_sbuf;
struct msgmbuf{
	int mtype;
	char mtext[10];
}msg_rbuf;

#define MSGPATH "/vagrant/interprocess/linux-ipcs-master/linux-ipcs-master/XSI_msgq/test"

int main()
{
	int gflags, sflags, rflags;
	key_t key;
	int msgid;
	int reval;
	struct msqid_ds msg_ginfo, msg_sinfo;

	key = ftok(MSGPATH, 'a');
	gflags = IPC_CREAT|IPC_EXCL;
	if ((msgid = msgget(key, gflags|0666)) == -1)
	{
		perror("msgget error\n");
	}
	//ouput default attribute after a queue created.
	msg_stat(msgid, msg_ginfo);
	
	sflags = IPC_NOWAIT;
	msg_sbuf.mtype=10;
	msg_sbuf.mtext[0] = 'a';
	if ((reval = msgsnd(msgid, &msg_sbuf, sizeof(msg_sbuf.mtext), sflags)) == -1)
	{
		perror("message send error\n");
	}
	//output attribute after a message sended.
	msg_stat(msgid, msg_ginfo);
	
	rflags = IPC_NOWAIT|MSG_NOERROR;
	if((reval = msgrcv(msgid, &msg_rbuf, 4, 10, rflags)) == -1)
		perror("read msg error\n");
	else
		printf("read from msg queue %d bytes\n", reval);
	//output attribute after a message readed.
	msg_stat(msgid, msg_ginfo);
	
	msg_sinfo.msg_perm.uid = 7;//should be useless
	msg_sinfo.msg_perm.gid = 7;//super user can change msg_qbytes.
	msg_sinfo.msg_qbytes = 16388;//larger than default.
	if ((reval = msgctl(msgid, IPC_SET, &msg_sinfo)) == -1)
	{
		perror("msg set info error\n");
		printf("\n");
	}
	//output super user changes.
	msg_stat(msgid, msg_ginfo);
	
	//delete message queue.
	if ((reval = msgctl(msgid, IPC_RMID, NULL)) == -1)
	{
		perror("unlink msg queue error\n");
	}
	
	
	return 0;	 
}

void msg_stat(int msgid, struct msqid_ds msg_info)
{
	int reval;
	sleep(1);
	
	if ((reval = msgctl(msgid, IPC_STAT, &msg_info)) == -1)
	{
		perror("get msg info error\n");
		return;
	}
	
	printf("\n");
	printf("bytes on queue is: %d\n", msg_info.msg_cbytes);
	printf("messages in queue is: %d\n", msg_info.msg_qnum);
	printf("max number of bytes on queue is %d\n", msg_info.msg_qbytes);
	//msg_qbytes' default is MSGMNB, which limit the capacity of queue and diff from system to system.
    printf("pid of last msgsnd is %d\n", msg_info.msg_lspid);
    printf("pid of last msgrcv is %d\n", msg_info.msg_lrpid);
    printf("last msgsnd time is %s", ctime(&(msg_info.msg_stime)));
    printf("last msgrcv time is %s", ctime(&(msg_info.msg_rtime)));
    printf("last change occurred %s", ctime(&(msg_info.msg_ctime)));
    printf("msg uid is %d\n", msg_info.msg_perm.uid);
    printf("msg gid is %d\n", msg_info.msg_perm.gid);
}
