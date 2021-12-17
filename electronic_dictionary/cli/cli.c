#include "cli.h"


static sqlite3 *db=NULL;
static char *errmsg=NULL;
static data a;


int ser_init()
{
	sqlite_init();
    int sfd=socket(AF_INET,SOCK_STREAM,0);
    if(sfd<0)
    {
        ERR_MSG("socket");
        return -1;
    }
    int reuse=1;
    if(setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int))<0)
    {
        ERR_MSG("setsockopt");
        return -1;
    }
    struct sockaddr_in sin;
    sin.sin_family=AF_INET;
    sin.sin_port=htons(PORT);
    sin.sin_addr.s_addr=inet_addr(IP);

    if(bind(sfd,(struct sockaddr *)&sin,sizeof(sin))<0)
    {
        ERR_MSG("bind");
        return -1;
    }
    if(listen(sfd,10)<0)
    {
        ERR_MSG("listen");
        return -1;
    }
    struct sockaddr_in cin;
    socklen_t a=sizeof(cin);
    pthread_t tid;

    while(1)
    {
        int newfd=accept(sfd,(struct sockaddr *)&cin,&a);
        if(newfd<0)
        {
            ERR_MSG("accept");
            return -1;
        }
        printf("[%s:%d] newfd=%d 链接成功\n" , inet_ntoa(cin.sin_addr), ntohs(cin.sin_port), newfd);//主线程用于处理链接问题
        struct cliInfo cli;
        cli.newfd=newfd;
        if(pthread_create(&tid,NULL,syst,(void *)&cli)!=0)
        {
            ERR_MSG("pthread_create");
            return -1;
        }
    }
    close(sfd);
    return 0;
}

void * syst(void *arg)
{
    pthread_detach(pthread_self());
    struct cliInfo cli=*(struct cliInfo *)arg;
    int newfd=cli.newfd;
    ssize_t res =0;
    data y;
	char buf[sizeof(y)]={0};
	while(1)
	{
		res=recv(newfd,buf,sizeof(buf),0);//接受客户端信息
		if(res<0){
			ERR_MSG("recv");
			break;
		}
		else if(0==res)//判断客户端是否退出
		{
			fprintf(stderr,"客户端关闭\n");
			break;
		}
		y= *(data *)buf;
		if(y.type=='0'||y.type=='1')
		{
			do_register(newfd,y);
		}
		else if(y.type=='3')
		{
			do_login(newfd,y);
		}
	}
	close(newfd);
	pthread_exit(NULL);
}

int do_exit(int newfd,data log)
{
	char sqp[200]="";
	char ** result=NULL;        
	int row,column;             
	char * errmsg=NULL;         

	sprintf(sqp,"update stu2 set online='F' where password='%s';",a.data2.account);                    
	if(sqlite3_get_table(db,sqp,&result,&row,&column,&errmsg)!=SQLITE_OK)                                
	{                                                                                                    
		printf("%s\n",sqlite3_errmsg(db));                                                               
		return -1;                                                                                       
	}
	log.type='1';
	if(send(newfd,(void *)&log,sizeof(log),0)<0)//发送信息给客户端
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;
}
int do_login(int newfd,data log)
{
	char sql[200]="select *from stu2;";
	char sqp[200]="";
	char ** result=NULL;
	int row,column;
	char * errmsg=NULL;
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));                                      
		return -1;
	}
	if(row!=0)
	{
		for(int i=0;i<row*column;i++)
		{
			if(i%4==2){
				if(strcmp(result[4+i],log.data2.account)==0)
				{
					if(strcmp(result[5+i],log.data2.coded)==0)
					{
						if(strcmp(result[i+3],"F")==0)
						{
							if(strcmp(result[i+2],"T")==0)
							{
								log.type='1';
								break;
							}
							else
							{
								log.type='0';
								break;
							}
						}
					}
				}
			}
			log.type='4';
		}
	}
	else
	{
		log.type='4';
	}
	if(log.type=='1'||log.type=='0')
	{
		a=log;
		sprintf(sqp,"update stu2 set online='T' where password='%s';",log.data2.account);
		if(sqlite3_get_table(db,sqp,&result,&row,&column,&errmsg)!=SQLITE_OK)
		{
			printf("%s\n",sqlite3_errmsg(db));
			return -1;
		}
	}
	if(send(newfd,(void *)&log,sizeof(log),0)<0)//发送信息给客户端
	{
		ERR_MSG("send");
		return -1;
	}
	if(log.type=='1')
	{
		do_system(newfd,log);
	}
	else if(log.type=='0')
	{
		work(newfd,log);
	}
	return 0;
}


int do_system(int newfd,data log)
{
	ssize_t res =0;                                        
    data y;                                                
    char buf[sizeof(y)]={0};                               
PP:                                                        
PPX:              
OO:
O1:
    while(1)                                               
    {                                                      
        res=recv(newfd,buf,sizeof(buf),0);//接受客户端信息 
        if(res<0){                                         
            ERR_MSG("recv");                               
            break;                                         
        }                                                  
        else if(0==res)//判断客户端是否退出                
        {                                                  
            fprintf(stderr,"客户端关闭\n");                
            break;                                         
        }                                                  
        y= *(data *)buf;                                   
        if(y.type==20)                       
        {                                                  
            system_add(newfd,y);                          
            goto PP;                                       
        }                                                  
        else if(y.type==23)                               
        {   
			system_change(newfd,y);
            goto PPX;                                      
        }                                                  
        else if(y.type=='9')                               
        {   
			do_exit(newfd,y);
			return 0;
        } 
		else if(y.type==25)
		{
			system_find(newfd,y);
			goto OO;
		}
		else if(y.type==26)
		{
			system_del(newfd,y);
			goto O1;
		}
    }                                                      
	return 0;

}
int system_del(int newfd,data log)
{
	char sql[200]={0};
	char ** result=NULL;
	int row,column;
	char * errmsg=NULL;
	sprintf(sql,"select * from stu1 where id=\"%s\";",log.data1.ID);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	if(row==0)
	{
		log.type='0';
		goto OO;
	}
	memset(sql,0,sizeof(sql));
	sprintf(sql,"delete from stu1 where id=\"%s\";",log.data1.ID);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	memset(sql,0,sizeof(sql));
	sprintf(sql,"delete from stu2 where password=\"%s\";",log.data1.ID);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}

OO:
	if(send(newfd,(void *)&log,sizeof(log),0)<0)//发送信息给客户端
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;
}
int system_find(int newfd,data log)
{
	char sql[200]={0};
	char ** result=NULL;
	int row,column;
	char * errmsg=NULL;
	sprintf(sql,"select * from stu1 where id=\"%s\";",log.data1.ID);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	if(row!=0)
	{                                                                                                           
		strcpy(log.data1.brr,result[column]);
		log.data1.sex=*result[column+1];	
		strcpy(log.data1.year,result[column+2]);
		strcpy(log.data1.addr,result[column+3]);
		strcpy(log.data1.age,result[column+4]);
		strcpy(log.data1.ID,result[column+5]);
		strcpy(log.data1.phone,result[column+6]);
	}
	else
	{
		log.type='0';	
	}
	if(send(newfd,(void *)&log,sizeof(log),0)<0)//发送信息给客户端
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;
}
int system_change(int newfd,data log)
{
	char sql[200]={0};
	char ** result=NULL;
	int row,column;
	char * errmsg=NULL;
    data y;
	y=log;
	sprintf(sql,"select * from stu1 where id=\"%s\";",log.data1.ID);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	if(row!=0)
	{                                                                                                           
		strcpy(log.data1.brr,result[column]);
		log.data1.sex=*result[column+1];	
		strcpy(log.data1.year,result[column+2]);
		strcpy(log.data1.addr,result[column+3]);
		strcpy(log.data1.age,result[column+4]);
		strcpy(log.data1.ID,result[column+5]);
		strcpy(log.data1.phone,result[column+6]);
	}
	else
	{
		log.type=24;
		goto OO;
	}
	
	bzero(sql,sizeof(sql));
	sprintf(sql,"delete from stu1 where name=\"%s\";",log.data1.brr);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}

	bzero(sql,sizeof(sql));	
	sprintf(sql,"insert into stu1 values(\"%s\",\"%c\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");",y.data1.brr,y.data1.sex,y.data1.year,y.data1.addr,y.data1.age,y.data1.ID,y.data1.phone);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}

OO:
	if(send(newfd,(void *)&log,sizeof(log),0)<0)//发送信息给客户端
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;
}
int system_add(int newfd,data log)
{
	char sql[200]="select id from stu1;";
	char ** result=NULL;
	int row,column;
	char * errmsg=NULL;
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	if(row!=0)
	{                                                                                                           
		for(int i=0;i<row;i++)
		{
			if(strcmp(result[i+1],log.data1.ID)==0)
			{
				log.type=22;
				goto PPQ;
			}
		}
		log.type=21;
	}
	else
	{
		log.type=21;	
	}
	if(log.type==21)
	{
		bzero(sql,sizeof(sql));
		sprintf(sql,"insert into stu1 values(\"%s\",\"%c\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");",log.data1.brr,log.data1.sex,log.data1.year,log.data1.addr,log.data1.age,log.data1.ID,log.data1.phone);
		if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) !=0)
		{
			printf("sqlite3_exec:%s %d\n", errmsg, __LINE__);
			return -1;
		}
	}
PPQ:
	if(send(newfd,(void *)&log,sizeof(log),0)<0)//发送信息给客户端
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;
}

int work(int newfd,data log)
{
    ssize_t res =0;                                        
    data y;                                                
    char buf[sizeof(y)]={0};                               
PP:                                                        
PPX:              
OO:
    while(1)                                               
    {                                                      
        res=recv(newfd,buf,sizeof(buf),0);//接受客户端信息 
        if(res<0){                                         
            ERR_MSG("recv");                               
            break;                                         
        }                                                  
        else if(0==res)//判断客户端是否退出                
        {                                                  
            fprintf(stderr,"客户端关闭\n");                
            break;                                         
        }                                                  
        y= *(data *)buf;                                   
        if(y.type=='7')                       
        {                                                  
            work_add(newfd,y); 
            goto PP;                                       
        }                                                  
        else if(y.type=='a')                               
        {   
			work_change(newfd,y);
            goto PPX;                                      
        }                                                  
        else if(y.type=='9')                               
        {   
			do_exit(newfd,y);
			return 0;
        } 
		else if(y.type=='c'||y.type=='d')
		{
			work_find(newfd,y);
			goto OO;
		}
    }                                                      
	return 0;
}

int work_find(int newfd,data log)
{
	
	char sql[200]={0};
	char ** result=NULL;
	int row,column;
	char * errmsg=NULL;
	sprintf(sql,"select * from stu1 where id=\"%s\";",log.data1.ID);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	if(row!=0)
	{                                                                                                           
		strcpy(log.data1.brr,result[column]);
		log.data1.sex=*result[column+1];	
		strcpy(log.data1.year,result[column+2]);
		strcpy(log.data1.addr,result[column+3]);
		strcpy(log.data1.age,result[column+4]);
		strcpy(log.data1.ID,result[column+5]);
		strcpy(log.data1.phone,result[column+6]);
	}
	else
	{
		log.type='0';	
	}
	if(send(newfd,(void *)&log,sizeof(log),0)<0)//发送信息给客户端
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;
}

int work_change(int newfd,data log)
{
	char sql[200]={0};
	char ** result=NULL;
	int row,column;
	char * errmsg=NULL;
    data y;
	y=log;
	sprintf(sql,"select * from stu1 where id=\"%s\";",log.data1.ID);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	if(row!=0)
	{                                                                                                           
		strcpy(log.data1.brr,result[column]);
		log.data1.sex=*result[column+1];	
		strcpy(log.data1.year,result[column+2]);
		strcpy(log.data1.addr,result[column+3]);
		strcpy(log.data1.ID,result[column+5]);
		strcpy(log.data1.phone,result[column+6]);
	}
	else
	{
		log.type='0';	
		goto OO;
	}
	bzero(sql,sizeof(sql));
	sprintf(sql,"update stu1 set sex=\"%c\" where name=\"%s\";",y.data1.sex,log.data1.brr);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	bzero(sql,sizeof(sql));
	sprintf(sql,"update stu1 set age=\"%s\" where name=\"%s\";",y.data1.year,log.data1.brr);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	bzero(sql,sizeof(sql));
	sprintf(sql,"update stu1 set addr=\"%s\" where name=\"%s\";",y.data1.addr,log.data1.brr);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}

	bzero(sql,sizeof(sql));
	sprintf(sql,"update stu1 set phone=\"%s\" where name=\"%s\";",y.data1.phone,log.data1.brr);
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}

OO:
	if(send(newfd,(void *)&log,sizeof(log),0)<0)//发送信息给客户端
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;
}

int work_add(int newfd,data log)
{
	char sql[200]="select id from stu1;";
	char ** result=NULL;
	int row,column;
	char * errmsg=NULL;
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	if(row!=0)
	{                                                                                                           
		for(int i=0;i<row;i++)
		{
			if(strcmp(result[i+1],log.data1.ID)==0)
			{
				log.type='6';
				goto PPQ;
			}
		}
		log.type='5';
	}
	else
	{
		log.type='5';	
	}
	if(log.type=='5')
	{
		bzero(sql,sizeof(sql));
		sprintf(sql,"insert into stu1 values(\"%s\",\"%c\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\");",log.data1.brr,log.data1.sex,log.data1.year,log.data1.addr,log.data1.age,log.data1.ID,log.data1.phone);
		if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) !=0)
		{
			printf("sqlite3_exec:%s %d\n", errmsg, __LINE__);
			return -1;
		}
	}
PPQ:
	if(send(newfd,(void *)&log,sizeof(log),0)<0)//发送信息给客户端
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;

}

int do_register(int newfd,data log)
{
	char sql[200]="select password from stu2;";
	char ** result=NULL;
	int row,column;
	char * errmsg=NULL;
	char b='F';
	char a='T';
	if(log.type=='0')
	{
		a='F';
	}else if(log.type=='1')
	{
		a='T';
	}
	if(sqlite3_get_table(db,sql,&result,&row,&column,&errmsg)!=SQLITE_OK)
	{
		printf("%s\n",sqlite3_errmsg(db));
		return -1;
	}
	if(row!=0)
	{                                                                                                           
		for(int i=0;i<row;i++)
		{
			if(strcmp(result[i+1],log.data2.account)==0)
			{
				log.type='2';
				goto PPQ;
			}
		}
	}
	if(log.type=='1'||log.type=='0')
	{
		bzero(sql,sizeof(sql));
		sprintf(sql,"insert into stu2 values(\"%c\",\"%c\",\"%s\",\"%s\");",a,b,log.data2.account,log.data2.coded);
		if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) !=0)
		{
			printf("sqlite3_exec:%s %d\n", errmsg, __LINE__);
			return -1;
		}
	}
PPQ:
	if(send(newfd,(void *)&log,sizeof(log),0)<0)//发送信息给客户端
	{
		ERR_MSG("send");
		return -1;
	}
	return 0;
}

int sqlite_init()
{
	if(sqlite3_open("./mysqlite1",&db)!=SQLITE_OK)
	{
		printf("ERROR: %s\n", sqlite3_errmsg(db));
		return -1;
	}
	char sql[128] = "create table if not exists stu1(name char, sex char, age char,addr char,pay char,id char primary key,phone char)";
	char sql1[128]="create table if not exists stu2(root char, online char,password char primary key,code char)";

	if(sqlite3_exec(db, sql, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("ERROR:%s __%d__\n", errmsg, __LINE__);
		return -1;
	}
	if(sqlite3_exec(db, sql1, NULL, NULL, &errmsg) != SQLITE_OK)
	{
		printf("ERROR:%s __%d__\n", errmsg, __LINE__);
		return -1;
	}
	return 0;
}
