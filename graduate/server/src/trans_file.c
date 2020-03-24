#include"trans_file.h"

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



int send_file(int new_fd,int fd,off_t offset){
	printf("start send\n");
//	int fd=open(filename,O_RDONLY);
//	if(-1==fd){
//		perror("open");
//		return -1;
//	}
	if(offset){
		printf("send file from last position\n");
		lseek(fd,offset,SEEK_SET);
	}
	train t;
	int ret;
	struct stat file_stat; 
	//先传送文件名
//	strcpy(t.buf,filename);
//	t.len=strlen(t.buf);
//	printf("name=%s len=%d\n",t.buf,t.len);
//	ret=sendn(new_fd,(char *)&t,t.len+sizeof(int));
//	if(-1==ret){
//		return -1;
//	}
//	
	fstat(fd,&file_stat);
	//send file size 
	//trans mode:  read / mmap bigger than 128M
	int size=(int)file_stat.st_size;
   	int  sended=0;	
	if(size<PAGESIZE*32||offset!=0){  //read  
		printf("send by read\n");
		while(bzero(&t,sizeof(train)),(t.len=read(fd,t.buf,sizeof(t.buf)))>0){
			ret=sendn(new_fd,(char *)&t,sizeof(int)+t.len);
			if(-1==ret){
				return -1;
			}
		}
	}else{
		printf("send by mmap\n");
		char * maddr=(char *)mmap(NULL,size,PROT_READ,MAP_PRIVATE,fd,0);
		while(bzero(&t,sizeof(train)),sended<size){
			if(file_stat.st_size-sended>sizeof(t.buf)){  
				memcpy(t.buf,maddr,sizeof(t.buf));
				t.len=sizeof(t.buf);
				maddr+=sizeof(t.buf);
				sended+=sizeof(t.buf);
			}else{
				int sub = size -sended;
				memcpy(t.buf,maddr,sub);
				t.len=sub;
				maddr+=sub;
				sended+=sub;
			}
			sendn(new_fd,(char *)&t,sizeof(int)+t.len);
		}
	}
	t.len=0;
	sendn(new_fd,(char*)&t,sizeof(int));
	printf("send over\n");
	return 0;
}

int download_file(int new_fd,char *md5){
	printf("start recv\n");
	char buf[BUFSIZE]={0};
	int len,ret;
	char path[BUFSIZE];
	bzero(buf,sizeof(buf));
	bzero(path,sizeof(path));
	sprintf(path,"../warehouse/%s",md5);
	puts(path);
	int fd=open(path,O_RDWR|O_CREAT,0644);
	if(-1==fd){
		perror("open");
		return -1;
	}
	while(1){
		ret=recvn(new_fd,(char *)&len,sizeof(int));
		if(-1==ret)return -1;
		if(len>0){
			bzero(buf,sizeof(buf));
			recvn(new_fd,buf,len);
			ret=write(fd,buf,len);
			if(-1==ret){
				perror("recv/write");
				return -1;
			}
		}else{
			break;
		}	
	}
	printf("recv over\n");	
	close(fd);
	return 0;  //download success
}
