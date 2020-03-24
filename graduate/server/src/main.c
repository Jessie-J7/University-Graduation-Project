#include"factory.h"

void sig(int signum){
	printf("exiting\n");
}

void * thread_function(void *p){
	char buf[1000];
	int ret=0;
	pfac pf =(pfac)p;
	pnode_t ptsk;
	char usr[256];
	int work_dir;   //work dir code 
	int root;
	char cmd_list[][10]={"CD","LS","PUTS","GETS","REMOVE","PWD","LOGIN","EXIT","MKDIR"};
	while(1){
		work_dir=-1;
		root=-1;
		bzero(buf,sizeof(buf));
		bzero(usr,sizeof(usr));
		pthread_mutex_lock(&pf->tsk_pque->lock);
		if(NULL==pf->tsk_pque->phead){
			pthread_cond_wait(&pf->cond,&pf->tsk_pque->lock);
		
		}
#ifdef FD_DEBUG
	printf("pf=%p \n",pf);
	printf("thread_num=%d\n",pf->thread_num);
	printf("tsk_pque=%p \n",pf->tsk_pque);
	printf("phead=%p \n",pf->tsk_pque->phead);
	printf("que_size=%d\n",pf->tsk_pque->que_size);
	printf("new_fd=%d in fac\n-------------------\n",pf->tsk_pque->phead->new_fd);
#endif
		get_que(pf->tsk_pque,&ptsk);
		pthread_mutex_unlock(&pf->tsk_pque->lock);
		//do something
		int cmd_type,len;
		cmd_msg msg;			
		train err_msg;
		while(1){
			bzero(&err_msg,sizeof(err_msg));
			bzero(&msg,sizeof(cmd_msg));
			ret=recvn(ptsk->new_fd,(char *)&msg,sizeof(cmd_msg));
			if(-1==ret){
				break;
			}
			sys_opt(msg.cmd_type);
		
			switch(msg.cmd_type){
				case 0:ret=cmd_cd(ptsk->new_fd,&work_dir,usr,root);break;   //cd
				case 1:ret=cmd_ls(ptsk->new_fd,work_dir,usr);break;
				case 2:ret=cmd_puts(ptsk->new_fd,work_dir,usr);break;
				case 3:ret=cmd_gets(ptsk->new_fd,work_dir,msg.addr,msg.offset);
					   break;
				case 4:ret=cmd_remove(ptsk->new_fd,work_dir,msg.addr,usr);break;
				case 5:ret=cmd_pwd(ptsk->new_fd,root);break;
				case 6:ret=login(ptsk->new_fd,usr,&root);
					   work_dir=root;
					   break;//login
				case 7:break;//exit
				case 8:ret=add_dir(work_dir,msg.addr,usr);break;
				default:;
			}
			if(-1==ret){
				syslog(LOG_INFO,"Failed to execute command %s from usr %s\n",cmd_list[msg.cmd_type],usr);
				err_msg.len=0;
				sendn(ptsk->new_fd,(char*)&err_msg,sizeof(int));	
				continue;
			}else{
				syslog(LOG_INFO,"Execute command %s from usr %s\n",cmd_list[msg.cmd_type],usr);
			}
		}
end:
		close(ptsk->new_fd);
		syslog(LOG_INFO,"Connection was terminated");
		free(ptsk);
	}
}


int main(int argc,char *argv[]){
//	if(5!=argc){
//		printf("args:IP port thread_num capacity\n");
//		return -1;
//	}
	signal(SIGUSR1,sig);
	FILE *fp=fopen("../conf/server.conf","r+");
	char ch_ip_addr[30]={0};
	char ch_port[30]={0};
	char ch_thread_num[30]={0};
	char ch_capacity[30]={0};
	fgets(ch_ip_addr,30,fp);
	fgets(ch_port,30,fp);
	fgets(ch_thread_num,30,fp);
	fgets(ch_capacity,30,fp);
	//初始化队列和线程池
	factory f;
	que_t que;
	bzero(&que,sizeof(que_t));
	bzero(&f,sizeof(factory));
	int thread_num=atoi(ch_thread_num);
	int capacity=atoi(ch_capacity);
	f.tsk_pque=&que;
	que.que_capacity=capacity;
	f.th_func=thread_function;
	f.thread_num=thread_num;
	int ret=fac_init(&f);
	if(-1==ret){
		perror("fac_init");
		return -1;
	}
	ret=fac_start(&f);
	if(-1==ret){
		perror("fac_start");
		return -1;
	}	
	
	//服务器端初始化socket，监听网络端口
	struct sockaddr_in ser,clnt;
	ser.sin_family=AF_INET;
	ser.sin_port=htons(8888);
	ser.sin_addr.s_addr=INADDR_ANY;
	int sfd=socket(AF_INET,SOCK_STREAM,0);
    if(-1==sfd)
	{
		perror("socket");//error
		exit(-1);
	}
	int reuse=1;
	ret=setsockopt(sfd,SOL_SOCKET,SO_REUSEPORT,&reuse,sizeof(int));
	if(-1==ret){
		perror("setsockopt");
		return -1;
	}
	ret=bind(sfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
	if(-1==ret){
		perror("bind");
		return -1;
	}
	listen(sfd,thread_num);
	int new_fd;
	int len;
	pnode_t ptsk;
	while(1){
		ptsk=(pnode_t)calloc(1,sizeof(node_t));
		bzero(&clnt,sizeof(struct sockaddr_in));
		len=sizeof(struct sockaddr);
		new_fd=accept(sfd,NULL,NULL);				
		syslog(LOG_INFO,"client connected by IP:%s",inet_ntoa(clnt.sin_addr));
		if(-1==new_fd){
			perror("accept");
		syslog(LOG_INFO,"client connection from IP:%s was refused",inet_ntoa(clnt.sin_addr));
			return -1;
		}
		ptsk->new_fd=new_fd;
		set_que(f.tsk_pque,ptsk);
		pthread_cond_signal(&f.cond);
	}
	int i;
end:
	for(i=0;i<thread_num;i++){
		
	}
}
