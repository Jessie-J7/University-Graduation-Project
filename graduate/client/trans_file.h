#ifndef __TRANS_FILE_H__
#define __TRANS_FILE_H__
#define BUFSIZE 1000
#include"head.h"
int download_file(int);
int recvn(int,char *,int);
int sendn(int,char *,int);
int send_file(int,char *);
#endif
