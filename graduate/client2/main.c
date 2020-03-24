#include"head.h"
int main(int argc,char *argv[]){

	struct sockaddr_in ser,clnt;
	int work_dir=0;
	bzero(&ser,sizeof(struct sockaddr_in));
	ser.sin_family=AF_INET;
	ser.sin_port=htons(8888);
	ser.sin_addr.s_addr=inet_addr("192.168.86.128");
	int sfd=socket(AF_INET,SOCK_STREAM,0);
	if(-1==sfd)
	{
		perror("socket");//error
		exit(-1);
	}
	int ret=connect(sfd,(struct sockaddr*)&ser,sizeof(struct sockaddr));
	if(-1==ret){
		perror("connect");
		return -1;
	}
	char cmd[CMDSIZE]="Login";
	//login module
	ret=cmd_opt(cmd,sfd,&work_dir);
	if(-1==ret){
		printf("client exit\n");
		return -1;
	}
	//command module
	while(1){
		printf("client$>");
		fflush(stdout);
		bzero(cmd,sizeof(cmd));
		fgets(cmd,CMDSIZE,stdin);
		if('\n'==cmd[strlen(cmd)-1])cmd[strlen(cmd)-1]=0;
		ret=cmd_opt(cmd,sfd,&work_dir);
	}
}
