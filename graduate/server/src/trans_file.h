#ifndef __TRANS_FILE_H__
#define __TRANS_FILE_H__
#include"head.h"
#define BUFSIZE	4096 
#define PAGESIZE 4096

int download_file(int ,char *);
int recvn(int,char *,int);
int sendn(int,char *,int);
int send_file(int,int ,off_t);
#endif
