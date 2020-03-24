#ifndef __HEAD_H__
#define __HEAD_H__
#include<stdio.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/types.h>
#include<signal.h>
#include<string.h>
#include<pthread.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include<syslog.h>
#include<sys/stat.h>
#include<sys/mman.h>
#include<mysql/mysql.h>
#include<openssl/md5.h>
#define CMDSIZE 200
typedef struct{
 	int len;
	char buf[1000];
}train;
#endif


