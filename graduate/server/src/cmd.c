#include"cmd.h"
int cmd_remove(int new_fd,int work_dir,char *filename,char *usr){
	train msg;
	int ret=0,linked,fileid;
	char md5[33]={0};
	bzero(&msg,sizeof(train));
	get_md5_byname(work_dir,filename,usr,md5);
	ret=get_link(md5,&linked,&fileid);
	if(1==ret){
		update_link(fileid,--linked,md5);
		rm_file(work_dir,filename);
		msg.len=0;
		sendn(new_fd,(char *)&msg,sizeof(int));	
		return 0;
	}else if(0==ret){ //have no such file
		msg.len=0;
		sendn(new_fd,(char *)&msg,sizeof(int));	
		return -1;
	}

}
int cmd_puts(int new_fd,int work_dir,char *usr){
	train msg;
	int ret=0,linked,fileid;
	char md5[33]={0};
	char filename[BUFSIZE]={0};
	bzero(&msg,sizeof(train));
	//receive file name and md5 code  and search it by md5 in the file system,
#ifdef DEBUG
	printf("before recv file name\n");
#endif
	ret=recvn(new_fd,(char *)&msg.len,sizeof(int));
	if(-1==ret)return -1;
	ret=recvn(new_fd,msg.buf,msg.len);   
	if(-1==ret)return -1;
#ifdef DEBUG
	printf("after recv file name\n");
#endif
	sscanf(msg.buf,"%s %s",filename,md5);
#ifdef DEBUG
	printf("filename=%s md5=%s\n",filename,md5);
#endif
	//ret=has_file(md5);
	ret=get_link(md5,&linked,&fileid);
	if(1==ret){
		msg.len=0;
		sendn(new_fd,(char *)&msg,sizeof(int));	//send a msg to tell client whether send file or not 
		add_file(work_dir,filename,usr,md5);
		update_link(fileid,++linked,md5);
	}else if(0==ret){ //haven't had this file yet
		strcpy(msg.buf,"SEND");
		msg.len=strlen(msg.buf);
		sendn(new_fd,(char *)&msg,sizeof(int)+msg.len);
		ret=download_file(new_fd,md5);
		if(0==ret){
			ret=add_file(work_dir,filename,usr,md5);
			add_link(++linked,md5);
			if(-1==ret){
				syslog(LOG_ERR,"failed adding file into batabase");
				return -1;
			}
			return 0;
		}else{
			return -1;
		}
	}else{
		return -1;
	}
}


int cmd_gets(int new_fd,int work_dir,char *filename,off_t offset){
	int ret,fd=-1;
	train msg;
	bzero(&msg,sizeof(msg));
	get_file_fd(filename,work_dir,&fd);   //search from warehouse whether there is a file in usr current dir
	if(-1==fd){
		msg.len=0;
		ret=sendn(new_fd,(char *)&msg.len,sizeof(int));
		if(-1==ret)return -1;
		return -1;
	}else{
	//send filename at first
		strcpy(msg.buf,filename);
		msg.len=strlen(msg.buf);
#ifdef DEBUG
		printf("name=%s len=%d\n",msg.buf,msg.len);
#endif
		ret=sendn(new_fd,(char *)&msg,msg.len+sizeof(int));
		if(-1==ret)return -1;
		ret=send_file(new_fd,fd,offset);
		if(-1==ret){
#ifdef DEBUG
			printf("send file failed\n");
#endif
			return -1;
		}else if(0==ret){
			close(fd);
			return 0;
		}
	}

}
int cmd_pwd(int new_fd,int root){
	//receive current work directory code  
	train msg;
	int ret;
	ret=recvn(new_fd,(char*)&msg.len,sizeof(int));
	if(-1==ret)return -1;
	ret=recvn(new_fd,msg.buf,msg.len);
	if(-1==ret)return -1;
#ifdef DEBUG
	printf("code=%d \n",atoi(msg.buf));
#endif
	get_cwd(root,atoi(msg.buf),msg.buf);
	msg.len=strlen(msg.buf);
	//send pwd to client
	ret=sendn(new_fd,(char *)&msg,sizeof(int)+msg.len);
	if(-1==ret)return -1;
	return 0;
}

int cmd_ls(int new_fd,int work_dir,char *usr){
	char info[4096];    //It may be not big enough for numerous items in one dir
	bzero(info,sizeof(info));
	char *sp=info;
	int ret,code=0;
	train msg;
	bzero(&msg,sizeof(train));
	ret=recvn(new_fd,(char*)&msg.len,sizeof(int));
	if(-1==ret)return -1;
	ret=recvn(new_fd,msg.buf,msg.len);
	if(-1==ret)return -1;
	if(0==msg.len){   //ls current dir
		get_items(work_dir,usr,info);
	}else{
		get_code_bypath(&code,msg.buf,usr);
		get_items(code,usr,info);
	}
	int send=0;
#ifdef DEBUG
	printf("%s",info);
#endif
	while(send<strlen(info)){
#ifdef DEBUG
		printf("strlen=%d send=%d\n",strlen(sp),send);
#endif
		if(sizeof(msg.buf)-1<(strlen(sp))){
			strncpy(msg.buf,sp,sizeof(msg.buf)-1);
			send+=strlen(msg.buf);
			sp+=strlen(msg.buf);
		}else{
			strcpy(msg.buf,sp);
			send+=strlen(sp);
			sp+=strlen(sp);
		}
		msg.len=strlen(msg.buf);
		ret=sendn(new_fd,(char*)&msg,sizeof(int)+msg.len);
		if(-1==ret){
			return -1;
		}
	}
	msg.len=0;
	ret=sendn(new_fd,(char*)&msg.len,sizeof(int));


	if(-1==ret)return -1;
}

int cmd_cd(int new_fd,int *pwork_dir,char *usr,int root){    //assume the path is valid 
	//receive work dir which will move to
	train msg;
	int ret=0,code=0;
	char tmp[256];
	bzero(&msg,sizeof(train));
	bzero(&tmp,sizeof(tmp));
	ret=recvn(new_fd,(char*)&msg.len,sizeof(int));
	if(-1==ret)return -1;
	ret=recvn(new_fd,msg.buf,msg.len);
	if(-1==ret)return -1;
#ifdef DEBUG
	printf("path=%s\n",msg.buf);
#endif
	if(!strcmp(msg.buf,"..")){
		// receive a '..' represent  change to parent folder
		code=get_procode(*pwork_dir,tmp);
		if(-1!=code){
			*pwork_dir=code;
		}else{
			msg.len=0;
			sendn(new_fd,(char*)&msg.len,sizeof(int));
			return -1;
		}
	}else if(msg.buf[0]!='/'){
		// receive file relative to work dir
		ret=get_code_byname(*pwork_dir,&code,msg.buf,usr);
		if(-1==ret){
			msg.len=0;
			sendn(new_fd,(char*)&msg.len,sizeof(int));
			return -1;
		}
		*pwork_dir=code;
	}else if(!strcmp(msg.buf,"/")){
		*pwork_dir=root;
		code=root;
	}else{
		//receive absolute path
		ret=get_code_bypath(&code,msg.buf,usr);
		if(-1==ret){
			msg.len=0;
			sendn(new_fd,(char*)&msg.len,sizeof(int));
			return -1;
		}
		*pwork_dir=code;
	}
	//send new work dir code to client
	sprintf(msg.buf,"%d",code);
	msg.len=strlen(msg.buf);
	ret=sendn(new_fd,(char*)&msg,sizeof(int)+msg.len);
	if(-1==ret)return -1;
}
