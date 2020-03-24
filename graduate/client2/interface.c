#include "head.h"
int cmd_remove(int sfd){
	train msg;
	bzero(&msg,sizeof(msg));
	recvn(sfd,(char*)&msg.len,sizeof(int));
	if(0==msg.len){
		return 0;
	}

}
int cmd_puts(int sfd,char *filename){
	train msg;
	bzero(&msg,sizeof(msg));
	int fd=open(filename,O_RDONLY);
	char md5[33]={0};
	int ret=0;
	get_md5(fd,md5);
	//send filename and md5 code
	sprintf(msg.buf,"%s %s",filename,md5);
	msg.len=strlen(msg.buf);
#ifdef DEBUG
	printf("ctx=%s len=%d\n",msg.buf,msg.len);
#endif
	ret=sendn(sfd,(char *)&msg,msg.len+sizeof(int));
	if(-1==ret)return -1;
	recvn(sfd,(char*)&msg.len,sizeof(int));
	if(0==msg.len){
		printf("puts over\n");
		return 0;
	}
	ret=recvn(sfd,(char*)msg.buf,msg.len);
	if(strcmp(msg.buf,"SEND")){   //need send file 
		send_file(sfd,filename);
	}else{
		msg.len=0;
		sendn(sfd,(char *)&msg.len,sizeof(int));
	}
	return 0;

}
int get_md5(int fd,char *md5){
	unsigned char digest[16]={0};
	int scale=1024*1024;
	unsigned char *pchunk=(unsigned char *)calloc(1,scale);
	int len;
	MD5_CTX c;
	MD5_Init(&c);
	while(bzero(pchunk,sizeof(pchunk)),(len=read(fd,pchunk,scale))>0){
		MD5_Update(&c,pchunk,len);
	}
	MD5_Final(digest,&c);
	int i;
	char tmp[3]={0};
	for(i=0;i<16;i++){
		sprintf(tmp,"%02x",digest[i]);
		strcat(md5,tmp);
	}
	free(pchunk);
	return 0;
}
int  cmd_ls(int sfd,char *addr){
	int ret=0;
	int len=0;
	char  buf[BUFSIZE];
	char info[4096];
	int count=0;
	train msg;
	bzero(info,sizeof(info));
	bzero(&msg,sizeof(train));
	strcpy(msg.buf,addr);
	msg.len=strlen(msg.buf);
	ret=sendn(sfd,(char*)&msg,sizeof(int)+msg.len);
	if(-1==ret)return-1;
	while(1){
		len=0;
		bzero(&buf,sizeof(buf));
		ret=recvn(sfd,(char*)&len,sizeof(int));
#ifdef DEBUG
		printf("loop time=%d len=%d\n",count,len);
#endif
		if(-1==ret){
			printf("ls err\n");
			return -1;		
		}
		if(len>0){
			ret=recvn(sfd,buf,len);
			sprintf(info,"%s%s",info,buf);
		}else{
			break;
		}
		count++;
	}
	printf("%s",info);
}


int cmd_cd(int sfd,char *path,int *pwork_dir){
	train msg;
	int ret;
	//send path which will be changed to
	strcpy(msg.buf,path);
	msg.len=strlen(msg.buf);
	ret=sendn(sfd,(char*)&msg,sizeof(int)+msg.len);
	if(-1==ret)return -1;
#ifdef DEBUG
	printf("path len=%d go to path %s\n",msg.len,msg.buf);
#endif
	//receive current work dir code 
	ret=recvn(sfd,(char*)&msg.len,sizeof(int));
	if(-1==ret)return -1;
	ret=recvn(sfd,msg.buf,msg.len);
	if(-1==ret)return -1;
	*pwork_dir=atoi(msg.buf);
#ifdef DEBUG
	printf("word dir code=%d\n",*pwork_dir);
#endif
}

off_t is_exist(char *fileaddr){ //check the size of file in local dir
	DIR* dp=opendir(".");
	off_t found=0;
	struct stat fstat;
	struct dirent * dirp;
	while((dirp=readdir(dp))!=NULL){
		if(!strcmp(dirp->d_name,fileaddr)){
			stat(fileaddr,&fstat);
			found=fstat.st_size;
			break;
		}
	}
	return found;
}

int cmd_opt(char *cmd_str,int sfd,int *work_dir){
	char cmd[CMDSIZE];
	cmd_msg msg;
	bzero(&msg,sizeof(cmd_msg));
	bzero(cmd,sizeof(cmd));
	msg.cmd_type=-1;
	char cmd_list[][10]={"CD","LS","PUTS","GETS","REMOVE","PWD","LOGIN","EXIT","MKDIR"};
	sscanf(cmd_str,"%s %s",cmd,msg.addr);
	uppercase(cmd);
#ifdef DEBUG
	printf("cmd = %s  addr = %s\n",cmd,msg.addr);
#endif
	int i,ret;
	for(i=0;i<sizeof(cmd_list)/sizeof(*cmd_list);i++){
		if(!strcmp(cmd_list[i],cmd)){
			msg.cmd_type=i;
			break;
		}
	}
#ifdef DEBUG
	printf("cmd_type=%d,command=%s\n",msg.cmd_type,cmd_list[msg.cmd_type]);
#endif
	if(msg.cmd_type!=1 &&msg.cmd_type!=5 && msg.cmd_type!=6 && msg.cmd_type!=7 && msg.cmd_type!=8){
		if((msg.addr_len=strlen(msg.addr))==0){
			printf("error:no args\n");
			return -1;
	 	}
	}
	msg.offset=is_exist(msg.addr);

	sendn(sfd,(char*)&msg,sizeof(cmd_msg));
	switch(msg.cmd_type){
		case 0:ret=cmd_cd(sfd,msg.addr,work_dir);break;//CD
		case 1:ret=cmd_ls(sfd,msg.addr);break;//LS
		case 2:ret=cmd_puts(sfd,msg.addr);break;//PUTS
		case 3:download_file(sfd);break;//GETS
		case 4:ret=cmd_remove(sfd);break;//REMOVE
		case 5:ret=cmd_pwd(sfd,*work_dir);break;//PWD
		case 6:ret=login(sfd,work_dir);break;//login
		case 7:printf("GoodBye!\n");exit(0);break;//exit
		case 8:break;
		default:printf("invalid command\n");//OTHER
	}
	return ret;

}

int cmd_pwd(int sfd,int work_dir){
	train msg;
	int ret;
	bzero(&msg,sizeof(msg));
#ifdef DEBUG
	printf("work dir code=%d\n",work_dir);
#endif
	sprintf(msg.buf,"%d",work_dir);
	msg.len=strlen(msg.buf);
	ret=sendn(sfd,(char *)&msg,sizeof(int)+msg.len);
	if(-1==ret)return -1;
	ret=recvn(sfd,(char *)&msg.len,sizeof(int));
	if(-1==ret)return -1;
	ret=recvn(sfd,msg.buf,msg.len);
	printf("%s\n",msg.buf);
	return 0;
}

void uppercase(char str[]){
	int i;
	for(i=0;i<strlen(str);i++){
		if(str[i]>='a'&&str[i]<='z'){
			str[i]=str[i]-32;
		}
	}
		
}
