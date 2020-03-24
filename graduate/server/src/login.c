#include"login.h"

int  login(int new_fd,char *usr,int * pwork_dir){
	train log_info;
	char salt[256];
	int ret=0,i;
	char usrname[256];
	while(1){
		if(-1==ret)break;
		bzero(&log_info,sizeof(train));
		bzero(usrname,sizeof(usrname));
		bzero(salt,sizeof(salt));
		ret=recvn(new_fd,(char*)&log_info.len,sizeof(&log_info.len));  
		if(-1==ret)break;
		ret=recvn(new_fd,log_info.buf,log_info.len);	//receive usrname
		if(-1==ret)break;
		strcpy(usrname,log_info.buf);
#ifdef DEBUG
		printf("len=%d usrname=%s\n",log_info.len,log_info.buf);
#endif
		has_usr(usrname,salt,pwork_dir);    //search usr and get salt value and root dir code
		strcpy(log_info.buf,salt);
		log_info.len=strlen(log_info.buf);
#ifdef DEBUG
		printf("len=%d salt=%s\n",log_info.len,log_info.buf);
#endif
		ret=sendn(new_fd,(char*)&log_info,sizeof(int)+log_info.len);   //send salt value 
		if(-1==ret)break;
		if(!strcmp(log_info.buf,"no_such_usr")){
			syslog(LOG_INFO,"Login rufused:invalid user attempt to login");
			continue;
		}
		for(i=0;i<3;i++){
			ret=recvn(new_fd,(char*)&log_info.len,sizeof(&log_info.len));
			if(-1==ret)break;
			ret=recvn(new_fd,log_info.buf,log_info.len);    //receive ciphertext 
			if(-1==ret)break;
#ifdef DEBUG
			printf("cipher which is authed=%s\n",log_info.buf);
#endif
			if(CORRECT==passwd_auth(usrname,log_info.buf)){
				strcpy(log_info.buf,"passwd correct");				
				log_info.len=strlen(log_info.buf);
#ifdef DEBUG
				printf("len=%d massage which will be send=%s\n",log_info.len,log_info.buf);
#endif
				ret=sendn(new_fd,(char*)&log_info,sizeof(int)+log_info.len);      //send auth massage
				if(-1==ret){
					break;	
					printf("sending auth massage failed\n");
				}
				//send usr's root dir code 
				sprintf(log_info.buf,"%d",*pwork_dir);
				log_info.len=strlen(log_info.buf);
				ret=sendn(new_fd,(char*)&log_info,sizeof(int)+log_info.len);
				ret= AUTH_SUCCESS;  //auth success = 0
				strcpy(usr,usrname);
				goto end;
			}else{
				if(2==i){
					strcpy(log_info.buf,"Authenticity failed");
					log_info.len=strlen(log_info.buf);
#ifdef DEBUG
					printf("massage which will be send=%s\n",log_info.buf);
#endif
					ret=sendn(new_fd,(char*)&log_info,sizeof(int)+log_info.len);
					if(-1==ret)break;
					syslog(LOG_INFO,"Authenticity failed");
					ret=-1;
				}else{
					strcpy(log_info.buf,"passwd incorrect");
					log_info.len=strlen(log_info.buf);
#ifdef DEBUG
					printf("massage which will be send=%s\n",log_info.buf);
#endif
					ret=sendn(new_fd,(char*)&log_info,sizeof(int)+log_info.len);
					if(-1==ret)break;
				}
			}
		}
	}
end:
	return ret;
}




int passwd_auth(char* usrname,char * cipher){
		MYSQL *conn;
		MYSQL_RES *res;
		MYSQL_ROW row;
		int auth=0;
		char* server="localhost";
		char* user="root";
		char* password="123";
		char* database="file_server";
		char query[300]="select * from user where usrname ='";
		sprintf(query,"%s%s%s",query,usrname,"'");
		int t,r;
		conn=mysql_init(NULL);
		if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
			syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
			goto end;
		}else{
			syslog(LOG_INFO,"Connected to database");
		}
		t=mysql_query(conn,query);
		if(t){
			syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
			goto end;
		}else{
			syslog(LOG_INFO,"Query %s made",query);
			res=mysql_use_result(conn);
			if(res){
				row=mysql_fetch_row(res);
				if(NULL==row){			
					strcpy(cipher,"auth failed");
					auth= INCORRECT;
				}else{
					if(!strcmp(cipher,row[3])){
						auth= CORRECT;	
					}
				}
			}
			mysql_free_result(res);
		}
end:
		mysql_close(conn);
		return auth;
}
void has_usr(char * usrname, char * salt,int * pwork_dir){
		MYSQL *conn;
		MYSQL_RES *res;
		MYSQL_ROW row;
		char* server="localhost";
		char* user="root";
		char* password="123";
		char* database="file_server";
		char query[300]="select * from user where usrname ='";
		sprintf(query,"%s%s%s",query,usrname,"'");
		int t,r;
		conn=mysql_init(NULL);
		if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
			syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
			strcpy(salt,"database_error");
		}else{
			syslog(LOG_INFO,"Connected to database");
		}
		t=mysql_query(conn,query);
		if(t){
			syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
			strcpy(salt,"database_error");
		}else{
			syslog(LOG_INFO,"Query %s made",query);
			res=mysql_use_result(conn);
			if(res){
				row=mysql_fetch_row(res);
				if(NULL==row){			
					strcpy(salt,"no_such_usr");
				}else{
					strcpy(salt,row[2]);
					*pwork_dir=atoi(row[4]);
				}
			}
			mysql_free_result(res);
		}
		mysql_close(conn);
}
