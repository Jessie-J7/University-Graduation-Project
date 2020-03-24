#include"trans_file.h"
int send_file(int new_fd,char *filename){
	printf("start send\n");
	int fd=open(filename,O_RDONLY);
	if(-1==fd){
		perror("open");
		return -1;
	}
	train t;
	int ret,len;
	
	while(bzero(&t,sizeof(train)),(t.len=read(fd,t.buf,sizeof(t.buf)))>0){
		ret=sendn(new_fd,(char *)&t,sizeof(int)+t.len);
		if(-1==ret)return -1;
	}
	t.len=0;
	sendn(new_fd,(char*)&t,sizeof(int));
	close(fd);
	printf("send over\n");
	return 0;
}

int download_file(int new_fd){
	printf("start download\n");
	char buf[BUFSIZE]={0};
	int len,ret,file_size;
	struct stat fst;
	ret=recvn(new_fd,(char *)&len,sizeof(int));
	if(-1==ret){
		return -1;
	}
	ret=recvn(new_fd,buf,len);
	if(-1==ret){
		return -1;
	}
	int fd=open(buf,O_RDWR|O_CREAT,0644);
	if(-1==fd){
		perror("open");
		return -1;
	}
	if((ret=is_exist(buf))!=0){
		printf("download from last position\n");
		lseek(fd,ret,SEEK_SET);
	}
	while(1){
			ret=recvn(new_fd,(char *)&len,sizeof(int));
			if(-1==ret){
				return -1;
			}
			if(len>0){
				bzero(buf,sizeof(buf));
				recvn(new_fd,buf,len);
				ret=write(fd,buf,len);
				if(-1==ret){
					perror("download/write");
					return -1;
				}
			}else{
				break;
			}	
		}
	
	close(fd);
}


int recvn(int fd,char *buf,int len){
	int tot=0;
	int ret=0;
	while(tot<len){
		ret=recv(fd,buf+tot,len-tot,0);
		if(-1==ret){
			perror("recvn");
			return -1;
		}else if(0==ret){
			return -1;
		}
		tot+=ret;
	}
	return 0;

}


int sendn(int fd,char *buf,int len){
	int tot=0;
	int ret=0;
	while(tot<len){
		ret=send(fd,buf+tot,len-tot,0);
		tot+=ret;
		if(-1==ret){
			perror("sendn");
			return -1;
		}else if(0==ret){
			return -1;
		}
	}
	return 0;

}


