#ifndef __LOGIN_H__
#define __LOGIN_H__
#include"head.h"
#include"trans_file.h"
#define AUTH_SUCCESS 0
#define CORRECT 1
#define INCORRECT -1

void has_usr(char * , char *,int *);
int passwd_auth(char *,char *);
int  login(int,char* ,int*);
#endif
