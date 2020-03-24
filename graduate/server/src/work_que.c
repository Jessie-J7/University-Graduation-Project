#include"work_que.h"

int set_que(pque_t pque,pnode_t pnode){
	pthread_mutex_lock(&pque->lock);
	if(pque->que_capacity==pque->que_size){
		printf("full queue\n");
		return -1;
	}
	if(0==pque->que_size){
		pque->phead=pnode;
		pque->ptail=pnode;
		pque->que_size++;
	}else{
		pque->ptail->pnext=pnode;
		pque->ptail=pnode;
		pque->que_size++;
	}
	pthread_mutex_unlock(&pque->lock);

#ifdef FD_DEBUG
	printf("new_fd=%d que size=%d after enque\n",pque->phead->new_fd,pque->que_size);
#endif
}

int  get_que(pque_t pque,pnode_t *ppnode){
	if(0==pque->que_size){
		printf("empty que\n");
		return -1;
	}
#ifdef FD_DEBUG
	printf("new_fd=%d before deque\n",pque->phead->new_fd);
#endif
	*ppnode=pque->phead;
	pque->phead=pque->phead->pnext;
	pque->que_size--;
#ifdef FD_DEBUG
	printf("new_fd=%d after deque\n",(*ppnode)->new_fd);
#endif
}
