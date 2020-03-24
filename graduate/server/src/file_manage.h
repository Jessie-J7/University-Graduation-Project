#ifndef __FILE_MANAGE_H__
#define __FILE_MANAGE_H__
#include"head.h"
#include"trans_file.h"
int add_dir(int ,char * ,char *);
int get_procode(int ,char *);  //return procode and get name from result-valuing argument
int get_file_fd(char *,int ,int *);  //get file discreptor by filename and procode 
int get_cwd(int, int,char *);
int get_code_byname(int ,int *,char *,char*);    //get code by given name and procode
int get_code_bypath(int *,char *,char *); //get code number by given path
int get_md5_byname(int ,char *,char *,char *);
int get_link(char * ,int *,int*);
int get_items(int ,char * ,char *);
int get_md5(int,char*);
int add_file(int ,char * ,char * ,char *);
int update_link(int ,int,char* );
int add_link(int  ,char *);
int has_file(char *);  //search by md5 code
int rm_file(int ,char *);  //remove file from file system
#endif
