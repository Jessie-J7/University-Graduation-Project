#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#include"head.h"

#define CMDSIZE 200
#define CMD_NUM 8
typedef struct cmd_msg{
	int cmd_type;
	off_t offset;	
	int addr_len;
   	char addr[CMDSIZE];
}cmd_msg;
int cmd_opt(char *,int,int*);
off_t is_exist(char *);
int cmd_cd(int ,char *,int *);
int  cmd_ls(int sfd,char *addr);
int cmd_remove(int sfd);
int cmd_pwd(int ,int);
int get_md5(int ,char *);
void uppercase(char str[]);
#endif
