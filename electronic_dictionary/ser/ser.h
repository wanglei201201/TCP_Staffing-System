#ifndef SER_H
#define SER_H

#include <signal.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <unistd.h>
#include <stdlib.h>
#define PORT  6666
#define IP    "192.168.1.58"    //ifconfig 查看自己的Ubuntu IP地址

#define ERR_MSG(msg)    do{\
	fprintf(stderr, "__%d__ :", __LINE__);\
	perror(msg); \
}while(0)

struct son_data{
	char brr[30];
	char sex;
	char year[5];                  
	char addr[20];
	char age[7];
	char ID[15];
	char phone[18];
};
struct pass_data{
	char account[30];
	char coded[20];
};

typedef struct data3{
	char type;//1.system注册 0.普通员工注册 2.账号存在 3.登录 4.登录失败 5.成功 6.失败 7.增加
	struct son_data data1;
	struct pass_data data2;
}data;

int sockid;
typedef void (*sighandler_t)(int);
int cli_init();
int menu();
int do_register();
int do_login();
int do_exit();
int work_meun(data);
int system_meun(data);
int work_add(data);
int work_change(data);
int work_find(data);
int system_add();
int system_change();
int system_find();
int system_del();
#endif
