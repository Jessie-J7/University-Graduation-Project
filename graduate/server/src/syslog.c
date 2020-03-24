#include"syslog.h"


void sys_opt(int cmd_type){
	char cmd_list[][10]={"cd","ls","puts","gets","remove","pwd"};
	syslog(LOG_INFO,"client request:%s",cmd_list[cmd_type]);

}
