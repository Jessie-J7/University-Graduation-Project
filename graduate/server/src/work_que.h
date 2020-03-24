#ifndef __WORK_QUE_H__
#define __WORK_QUE_H__
#include"head.h"

typedef struct node{
	int new_fd;
	struct node *pnext;
}node_t,*pnode_t;

typedef struct que{
	pnode_t phead,ptail;
	int que_size;
	int que_capacity;
	pthread_mutex_t lock;
}que_t,*pque_t;

int set_que(pque_t ,pnode_t);
int get_que(pque_t,pnode_t*);
#endif
