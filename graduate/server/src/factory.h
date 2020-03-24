#ifndef __FACTORY_H__
#define __FACTORY_H__
#include"head.h"
#include"work_que.h"
#include"trans_file.h"
#include"syslog.h"
#include"cmd.h"
#include"login.h"
#include "file_manage.h"
#define STARTED 1
#define INITED 0
#define DESTROYED 2
typedef  void *(*pfunc)(void*);

typedef struct{
	pthread_cond_t cond;	
	pque_t tsk_pque;
	pthread_t *pthid;
	int status;
	int thread_num;
	pfunc th_func;
}factory,*pfac;

typedef struct cmd_msg{
	int cmd_type;
	off_t offset;	
	int addr_len;
   	char addr[CMDSIZE];
}cmd_msg;
int fac_init(pfac);
int fac_start(pfac);
#endif
