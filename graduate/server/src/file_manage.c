#include"file_manage.h"

//int add_dir(int work_dir,char * dirname,char * usr)

//int has_file(char *);  //search by md5 code 

//int get_md5(int fd,char *md5);

//int get_code_bypath(int *code,char *path,char *usr) //get code number by given path

//int get_code_byname(int procode,int *code,char * name,char * usr)    //get code by given name and procode

//int get_cwd(int root,int current,char *cwd)    //get current work dir by work dir code

//int get_procode(int code,char *name)  //return procode and get file/dir name from result-valuing argument

//int get_items(int work_dir,char * usr,char *info)

//int add_file(int work_dir,char * filename,char * usr,char *md5)

//int get_file_fd(char *filename,int work_dir,int *fd)  //get file discreptor by filename and procode 

int get_md5_byname(int procode,char *name,char *usr,char *md5){
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int ret;
	char* server="localhost";
	char* user="root";
	char* password="123";
	char* database="file_server";
	char query[300]="select * from file_system where type='f' and name ='";
	sprintf(query,"%s%s%s%d%s%s%s",query,name,"' and procode='",procode,"' and owner='",usr,"'");
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Connected to database");
	}
	t=mysql_query(conn,query);
	if(t){
		syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Query %s made",query);
		res=mysql_use_result(conn);
		if(res){
			row=mysql_fetch_row(res);
			if(NULL==row){			
				ret= -1;
			}else{
				strcpy(md5,row[5]);
				ret=0;
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return ret;
}

int rm_file(int work_dir,char *filename){  //remove file from file system
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="123";
	char* database="file_server";
	char query[BUFSIZE]="delete from file_system where procode=";
	sprintf(query,"%s %d and name='%s'",query,work_dir,filename);
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Connected to database");
	}
	t=mysql_query(conn,query);
	if(t){
		syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Query %s made",query);
		return 0;	
	}
	mysql_free_result(res);
	mysql_close(conn);
}	

int update_link(int fileid,int linked,char *md5 ){  //update storekeeper or remove it 
	if(linked){
		MYSQL *conn;
		MYSQL_RES *res;
		MYSQL_ROW row;
		char* server="localhost";
		char* user="root";
		char* password="123";
		char* database="file_server";
		char query[BUFSIZE]="update storekeeper set linked=";
		sprintf(query,"%s %d where fileID=%d",query,linked,fileid);
		int t,r;
		conn=mysql_init(NULL);
		if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
			syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
			return -1;
		}else{
			syslog(LOG_INFO,"Connected to database");
		}
		t=mysql_query(conn,query);
		if(t){
			syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
			return -1;
		}else{
			syslog(LOG_INFO,"Query %s made",query);
			return 0;	
		}
		mysql_free_result(res);
		mysql_close(conn);
	}else{  //link = 0, remove file from storekeeper and warehouse
		char path[BUFSIZE];
		sprintf(path,"../warehouse/%s",md5);
		unlink(path);
		MYSQL *conn;
		MYSQL_RES *res;
		MYSQL_ROW row;
		char* server="localhost";
		char* user="root";
		char* password="123";
		char* database="file_server";
		char query[BUFSIZE]="delete from storekeeper where fileid=";
		sprintf(query,"%s %d",query,fileid);
		int t,r;
		conn=mysql_init(NULL);
		if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
			syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
			return -1;
		}else{
			syslog(LOG_INFO,"Connected to database");
		}
		t=mysql_query(conn,query);
		if(t){
			syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
			return -1;
		}else{
			syslog(LOG_INFO,"Query %s made",query);
			return 0;	
		}
		mysql_free_result(res);
		mysql_close(conn);
	}
}

int add_link(int linked ,char *md5){
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="123";
	char* database="file_server";
	char query[BUFSIZE]="insert into storekeeper (md5,linked) values(";
	sprintf(query,"%s'%s','%d' )",query,md5,linked);
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Connected to database");
	}
	t=mysql_query(conn,query);
	if(t){
		syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Query %s made",query);
		return 0;	
	}
	mysql_free_result(res);
	mysql_close(conn);
}

int get_link(char * md5,int *linked,int *fileID){
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="123";
	char* database="file_server";
	char query[300]="select * from storekeeper where md5 ='";
	sprintf(query,"%s%s%s",query,md5,"'");
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Connected to database");
	}
	t=mysql_query(conn,query);
	if(t){
		syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Query %s made",query);
		res=mysql_use_result(conn);
		if(res){
			row=mysql_fetch_row(res);
			if(NULL==row){		
				*linked=0;
				return 0;  //don't have file
			}else{
				*fileID=atoi(row[0]);
				*linked=atoi(row[2]);
				return 1;  //has file 
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);

}



int get_file_fd(char *filename,int work_dir,int *fd){  //get file discreptor by filename and procode 
	char path[BUFSIZE];
	int ret;
	bzero(path,sizeof(path));
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="123";
	char* database="file_server";
	char query[300]="select * from file_system where procode  =";
	sprintf(query,"%s%d and name='%s'",query,work_dir,filename);
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Connected to database");
	}
	t=mysql_query(conn,query);
	if(t){
		syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Query %s made",query);
		res=mysql_use_result(conn);
		if(res){
			row=mysql_fetch_row(res);
			if(NULL==row){			
				ret= 0;  //don't have file
			}else{
				sprintf(path,"../warehouse/%s",row[5]);
				*fd=open(path,O_RDONLY);
				ret= 1;  //has file 
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return ret;
}

int add_dir(int work_dir,char * dirname,char * usr){
	struct stat buf;
	bzero(&buf,sizeof(buf));
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="123";
	char* database="file_server";
	char query[BUFSIZE]="insert into file_system(procode,name,owner,type,size) values(";
	sprintf(query,"%s %d,'%s','%s','d','4')",query,work_dir,dirname,usr);
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Connected to database");
	}
	t=mysql_query(conn,query);
	if(t){
		syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Query %s made",query);
		return 0;	
	}
	mysql_free_result(res);
	
	mysql_close(conn);
}
int add_file(int work_dir,char * filename,char * usr,char *md5){
	struct stat buf;
	bzero(&buf,sizeof(buf));
	char pathname[BUFSIZE];
	sprintf(pathname,"../warehouse/%s",md5);
	stat(pathname,&buf);
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="123";
	char* database="file_server";
	char query[BUFSIZE]="insert into file_system(procode,name,owner,type,md5,size) values(";
	sprintf(query,"%s %d,'%s','%s','f','%s',%ld)",query,work_dir,filename,usr,md5,buf.st_size/1024);
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Connected to database");
	}
	t=mysql_query(conn,query);
	if(t){
		syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Query %s made",query);
		return 0;	
	}
	mysql_free_result(res);
	
	mysql_close(conn);
}

int has_file(char * md5){  //search by md5 code
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="123";
	char* database="file_server";
	char query[300]="select * from file_system where md5 ='";
	sprintf(query,"%s%s%s",query,md5,"'");
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Connected to database");
	}
	t=mysql_query(conn,query);
	if(t){
		syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Query %s made",query);
		res=mysql_use_result(conn);
		if(res){
			row=mysql_fetch_row(res);
			if(NULL==row){		
				return 0;  //don't have file
			}else{
				return 1;  //has file 
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);

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
int get_items(int work_dir,char * usr,char *info){
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="123";
	char* database="file_server";
	char tmp[300];
	char query[300]="select size,type,owner,name from file_system where procode ='";
	sprintf(query,"%s%d%s%s%s",query,work_dir,"' and owner='",usr,"'");
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Connected to database");
	}
	t=mysql_query(conn,query);
	if(t){
		syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Query %s made",query);
		res=mysql_use_result(conn);
		if(res){
			while((row=mysql_fetch_row(res))!=NULL){
				bzero(tmp,sizeof(tmp));
				if(NULL==row){			
					return -1;
				}else{
					for(t=0;t<mysql_num_fields(res);t++){
						sprintf(tmp,"%s %s",tmp,row[t]);
					}
					sprintf(info,"%s%s\n",info,tmp);
				}
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}

int get_procode(int code,char *name){  //return procode and get file/dir name from result-valuing argument
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="123";
	char* database="file_server";
	char query[300]="select * from file_system where code =";
	sprintf(query,"%s%d",query,code);
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Connected to database");
	}
	t=mysql_query(conn,query);
	if(t){
		syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Query %s made",query);
		res=mysql_use_result(conn);
		if(res){
			row=mysql_fetch_row(res);
			if(NULL==row){			
				return -1;
			}else{
				if(NULL!=name){
					strcpy(name,row[2]);
				}
				return atoi(row[1]);
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);

}

int get_cwd(int root,int current,char *cwd){    //get current work dir by work dir code
	char cur_dir[256];
	char path[16][256];
	int i=0,ret;
	for(i=0;current!=root;i++){
#ifdef DEBUG 
		printf("loop=%d\n",i);
#endif
		current=get_procode(current,path[i]);
		if(-1==current){
			return -1;
		}
	}
	strcpy(cwd,"/");
	for(i--;i>=0;i--){
		strcat(cwd,path[i]);
		if(0!=i){
			strcat(cwd,"/");
		}
	}
	return 0;

}

int get_code_byname(int procode,int *code,char * name,char * usr){    //get code by given name and procode
	if(NULL==code){
		syslog(LOG_ERR,"get_code/bad address for write");
		return -1;
	}
	MYSQL *conn;
	MYSQL_RES *res;
	MYSQL_ROW row;
	char* server="localhost";
	char* user="root";
	char* password="123";
	char* database="file_server";
	char query[300]="select * from file_system where type='d' and name ='";
	sprintf(query,"%s%s%s%d%s%s%s",query,name,"' and procode='",procode,"' and owner='",usr,"'");
	int t,r;
	conn=mysql_init(NULL);
	if(!mysql_real_connect(conn,server,user,password,database,0,NULL,0)){
		syslog(LOG_ERR,"Error connecting to database:%s\n",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Connected to database");
	}
	t=mysql_query(conn,query);
	if(t){
		syslog(LOG_ERR,"Error making query:%s",mysql_error(conn));
		return -1;
	}else{
		syslog(LOG_INFO,"Query %s made",query);
		res=mysql_use_result(conn);
		if(res){
			row=mysql_fetch_row(res);
			if(NULL==row){			
				return -1;
			}else{
				*code=atoi(row[0]);
			}
		}
		mysql_free_result(res);
	}
	mysql_close(conn);
	return 0;
}
int get_code_bypath(int *code,char *path,char *usr){ //get code number by given path
	char delims[]="/";
	if(NULL==path){
		return -1;
	}
	int tempcode=*code;
	int ret;
	char * result=NULL;
	result=strtok(path,delims);
	ret=get_code_byname(tempcode,&tempcode,result,usr);
	printf("ret=%d\n",ret);
	if(-1==ret)return -1;
	while(NULL!=result){
		result=strtok(NULL,delims);
		get_code_byname(tempcode,&tempcode,result,usr);
	}
	*code=tempcode;
	return 0;
}
