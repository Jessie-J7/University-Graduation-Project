####安装提示
>安装Linux ubuntu 16.04版本。
>安装mysql:sudo apt-get install mysql-server mysql-client

>启动数据库：mysql –u root –p
>先新建数据库 名为‘file_server’ 
>create database file_server；
>使用数据库：use file_server； 
-------------------------------
####表1 "user"
'''
 CREATE TABLE `user` (
`usrID` int(11) NOT NULL AUTO_INCREMENT,
`usrname` varchar(256) NOT NULL,
`salt` varchar(256) NOT NULL,
`ciphertext` varchar(512) NOT NULL,
`root` int(11) DEFAULT NULL,
PRIMARY KEY (`usrID`),
UNIQUE KEY `usrname` (`usrname`)); 
'''

####表2 "file_server"
'''
 CREATE TABLE `file_system` (
 `code` int(11) NOT NULL AUTO_INCREMENT,
 `procode` int(11) NOT NULL,
 `name` varchar(256) NOT NULL,
 `owner` varchar(256) NOT NULL,
 `type` varchar(2) DEFAULT NULL,
 `md5` varchar(32) DEFAULT NULL,
  `size` bigint(20) DEFAULT NULL,
  PRIMARY KEY (`code`));
'''

####表3 "storekeeper"
'''
CREATE TABLE `storekeeper` (
`fileID` int(11) NOT NULL AUTO_INCREMENT,
`md5` varchar(33) DEFAULT NULL,					   
`linked` int(11) DEFAULT '0',					     
PRIMARY KEY (`fileID`));
'''
-------------------------------
####添加表项
'''
insert into user(usrname,salt,ciphertext,root) values('faith','$6$db.ODI3L$',' $6$db.ODI3L$ag50jYqT/OkZkBn91pmmjeIHx0qyHYUglPCw92yVC2Es40SWL2tc.gol6qYD6ZHGhLsyn38plYMlBTzUZidFb1','0');

insert into user(usrname,salt,ciphertext,root) values('rose',' $6$EBaqDHVp$',' $6$EBaqDHVp$53VomWKWUG/oXUmdQBAKATgCo9yU4ObiPryTOLgxKev.6XhOgqyqohtVc4xVj8vyODQD2bOj9WtqisrgpQWvh0','0');
'''
-------------------------------
####操作说明
*1.将代码放到Linux的目录下。Makefile实现自动化编译，服务器端：在src目录下输入make命令。
*2.客户端需要修改main.c中的 ip为当前服务器的ip。客户端：在client目录下输入make命令。
*3.服务器端：运行bin目录下的fileserver可执行文件；客户端：运行client可执行文件。
*4.用户faith 密码 khfaith7；用户 rose 密码123。
*5.输入以下命令进行服务器上的文件查看：
（1）、cd “目录名”：进入对应目录；
（2）、ls：列出相应目录文件；
（3）、puts “文件名”：将本地文件上传至服务器
（4）、gets“文件名”：下载服务器文件到本地；
（5）、remove “文件名”：删除服务器上文件
（6）、pwd：显示目前所在路径；
（7）、mkdir “目录名”：新建目录；
（8）、exit：客户端退出；
（9）、其他命令不响应；
