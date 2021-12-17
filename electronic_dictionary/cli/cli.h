#ifndef CLI_H
#define CLI_H

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>                                                                      
#include <signal.h>           
#include <sys/wait.h>
#include <stdlib.h>
#include <pthread.h>
#include <sqlite3.h>

#define PORT 6666
#define IP "192.168.1.58"

#define  ERR_MSG(msg) do{fprintf(stderr,"__%d__:",__LINE__);perror(msg);}while(0)
int ser_init();
struct cliInfo
{
	int newfd;
	struct sockaddr_in cin;
};

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
	char type;
	struct son_data data1;
	struct pass_data data2;
}data;
//volatile estatic sqlite3 *db=NULL;      
//volatile static char *errmsg=NULL;
//volatile static data a;



void * syst(void *arg);
int ser_init();
int sqlite_init();
int do_register(int newfd,data log);
int do_login(int newfd,data log);
int do_exit(int newfd,data log);
int work(int newfd,data log);
int work_add(int newfd,data log);
int work_change(int newfd,data log);
int work_find(int newfd,data log);
int do_system(int newfd,data log);
int system_add(int newfd,data log);
int system_change(int newfd,data log);
int system_find(int newfd,data log);
int system_del(int newfd,data log);


#endif
