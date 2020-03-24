#include"usr.h"
int  login(int sfd,int * pwork_dir){
	char usrname[256];
	char passwd[256];
	char salt[256];
	char * cipher=NULL;
	struct termios ts ,ots;
	tcgetattr(STDIN_FILENO,&ts);
	ots=ts;
	//cmd_msg msg;
	train log_info;
	int ret=0;
	while(1){
		cipher=NULL;
		bzero(usrname,sizeof(usrname));
		bzero(passwd,sizeof(passwd));
		bzero(&log_info,sizeof(train));
		bzero(salt,sizeof(salt));
		printf("login as: ");
		fflush(stdout);
		fgets(usrname,sizeof(usrname),stdin);
		if(usrname[strlen(usrname)-1]=='\n') usrname[strlen(usrname)-1]=0;
		strcpy(log_info.buf,usrname);
		log_info.len=strlen(log_info.buf);
		sendn(sfd,(char*)&log_info,sizeof(int)+log_info.len);	  //send usrname 
		bzero(&log_info,sizeof(train));  
		recvn(sfd,(char*)&log_info.len,sizeof(int));
		recvn(sfd,(char*)log_info.buf,log_info.len);     //receive salt value
		strcpy(salt,log_info.buf);
		if(!strcmp(log_info.buf,"no_such_usr")){
			printf("No Such User in System\n");
			continue;
		}
		ts.c_lflag &= ~ECHO;
		ts.c_lflag |= ECHONL;
		while(1){
			printf("%s's password: ",usrname);
			fflush(stdout);
			tcsetattr(STDIN_FILENO,TCSAFLUSH,&ts);
			fgets(passwd,sizeof(passwd),stdin);
			if(passwd[strlen(passwd)-1]=='\n') passwd[strlen(passwd)-1]=0;
			tcsetattr(STDIN_FILENO,TCSANOW,&ots);
			cipher =crypt(passwd,salt);
			log_info.len=strlen(cipher);
			strcpy(log_info.buf,cipher);
			sendn(sfd,(char*)&log_info,sizeof(int)+log_info.len);  //send ciphertext
			bzero(&log_info,sizeof(train));
			recvn(sfd,(char*)&log_info.len,sizeof(int)); 
			if(-1==ret)break;
			ret=recvn(sfd,(char*)log_info.buf,log_info.len);   //receive auth massage
			if(-1==ret)break;
			if(!strcmp(log_info.buf,"Authenticity failed")){
				printf("Authenticity failed\n");
				ret=-1;
				goto end;		
			}else if(!strcmp(log_info.buf,"passwd correct")){
				recvn(sfd,(char*)&log_info.len,sizeof(int)); 
				if(-1==ret)break;
				ret=recvn(sfd,(char*)log_info.buf,log_info.len);   //receive usr's root dir code
				if(-1==ret)break;
#ifdef DEBUG
				printf("got work_code=%s\n",log_info.buf);
#endif
				*pwork_dir=atoi(log_info.buf);
				goto end;
			}
		}
		
	}
end:
	return ret;
}

void salt_gen(char *salt,int len){
	srand(time(NULL));
	char ss[2]={0};
	char char_list[]="0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz./";
	int i;
	int setlen=strlen(char_list);
	for(i=0;i<len;i++){
		sprintf(ss,"%c",char_list[(rand()%setlen)]);
		strcat(salt,ss);
	}
	strcat(salt,"$");
	puts(salt);
}





