#include"factory.h"


int fac_init(pfac p){
	p->tsk_pque->que_size=0;
	int ret=pthread_cond_init(&p->cond,NULL);
	if(-1==ret){
		return -1;
	}
	ret=pthread_mutex_init(&p->tsk_pque->lock,NULL);
	if(-1==ret){
		return -1;
	}
	p->pthid =(pthread_t *)calloc(p->thread_num,sizeof(pthread_t));
	p->status=INITED;
	syslog(LOG_INFO,"initing factory success");
}


int fac_start(pfac p){
	int ret;
	int i;
	if(INITED==p->status){
		for(i=0;i<p->thread_num;i++){
			ret=pthread_create(&p->pthid[i],NULL,p->th_func,p);
			if(-1==ret){
				return -1;
			}
		}
		p->status=STARTED;
	}else{
		return -1;
	}
	syslog(LOG_INFO,"starting factory success");
}
