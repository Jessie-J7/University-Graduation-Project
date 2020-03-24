#ifndef __HEAD_H__
#define __HEAD_H__
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
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
#include<dirent.h>
#include<openssl/md5.h>
#include<termios.h>
#include<sys/time.h>
#include<crypt.h>
#include"trans_file.h"
#include"interface.h"
#include"usr.h"
#define FILENAME "file"
typedef struct{
 	int len;
	char buf[BUFSIZE];
}train;
#endif


