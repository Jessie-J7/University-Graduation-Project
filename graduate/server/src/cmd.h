#ifndef __CMD_H__
#define __CMD_H__
#include"head.h"
#include"file_manage.h"
#include"trans_file.h"
int cmd_cd(int,int * ,char *,int);
int cmd_pwd(int,int);
int cmd_remove(int ,int ,char *,char *);
int cmd_ls(int ,int ,char *);
int cmd_puts(int ,int ,char *);
int cmd_gets(int ,int ,char *,off_t);
#endif
