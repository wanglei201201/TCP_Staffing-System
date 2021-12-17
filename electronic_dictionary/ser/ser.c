#include "ser.h"

int cli_init()
{
	sockid=socket(AF_INET,SOCK_STREAM,0);//创建流式套接字
	if(sockid<0){
		ERR_MSG("sockid");
		return -1;
	}

	//允许端口快速重用                                                   
	int reuse =1;
	if(setsockopt(sockid,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int))<0)
	{
		ERR_MSG("setsockopt");
		return -1;
	}

	struct sockaddr_in sin;//绑定服务器的IP和端口，填充地址信息结构体
	sin.sin_family=AF_INET;
	sin.sin_port=htons(PORT);//端口号的网络字节序
	sin.sin_addr.s_addr=inet_addr(IP);//IP地址的网络字节序

	if(connect(sockid,(struct sockaddr*)&sin,sizeof(sin))<0)//连接服务器
	{
		ERR_MSG("connect");
		return -1;
	}                                                                           
	//char duf[108]="";
	//size_t size;
	menu();
	close(sockid);
	return 0;
}

int menu()
{
	int select;
	while(1)
	{
		system("clear");
		printf("*********员工管理系统*************\n");
		printf("************1.注册****************\n");
		printf("************2.登录****************\n");
		printf("************3.退出****************\n");
		printf("************V.177*****************\n");
		printf("input>>>>>");
		scanf("%d",&select);
		while(getchar()!=10);
		switch(select)
		{
		case 1:
			do_register();
			break;
		case 2:
			do_login();
			break;
		case 3:
			return 0;
			break;
		default:
			printf("input error\n");
		}
		printf("输入任意字符刷新页面>>>>");
		while(getchar()!=10);
	}
}
int do_exit()
{

	data x;
	size_t size=0;
	char buf[sizeof(x)];
	x.type='9';
	if(send(sockid,(void *)&x,sizeof(x),0)<0)         
	{                                                 
		ERR_MSG("send");                              
		return -1;                                    
	}                                                 
	size=recv(sockid,buf,sizeof(buf),0);//接收服务器的数据
	if(size<0){
		ERR_MSG("recv");
		return -1;
	}
	x=*(data *)buf;
	if(x.type=='1')
	{
		printf("感谢使用\n输入任意字符进行下一步\n");
		while(getchar()!=10);
	}
	return 0;
}
int do_register()
{
	data x;
	size_t size=0;
	char buf[sizeof(x)];

	printf("请问是否为管理员注册1.是/0.否>>>>>");
	x.type=getchar();
	printf("input password>>>>>");
	scanf("%s",x.data2.account);
	getchar();
	printf("input coded>>>>>");
	scanf("%s",x.data2.coded);
	getchar();
	if(send(sockid,(void *)&x,sizeof(x),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}

	size=recv(sockid,buf,sizeof(buf),0);//接收服务器的数据
	if(size<0){
		ERR_MSG("recv");
		return -1;
	}
	x=*(data *)buf;
	if(x.type=='2')
	{
		printf("您的账号已存在,请重新输入\n输入任意字符进行下一步\n");
		while(getchar()!=10);
		return -
			1;
	}
	else if(x.type=='0')
	{
		printf("注册成功\n输入任意字符进行下一步\n");
		while(getchar()!=10);
	}                                                                       
	else if(x.type=='1')
	{
		printf("system注册成功\n输入任意字符进行下一步\n");
		while(getchar()!=10);
	}                                                                       
	return 0;
}

int do_login()
{
	data x;
	size_t size=0;
	char buf[sizeof(x)];

	printf("input password>>>>>");
	scanf("%s",x.data2.account);
	while(getchar()!=10);
	printf("input coded>>>>>");
	scanf("%s",x.data2.coded);
	while(getchar()!=10);
	x.type='3';
	if(send(sockid,(void *)&x,sizeof(x),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	size=recv(sockid,buf,sizeof(buf),0);//接收服务器的数据
	if(size<0){
		ERR_MSG("recv");
		return -1;
	}
	x=*(data *)buf;
	if(x.type=='1')
	{
		//printf("system\n");
		system_meun(x);
	}else if(x.type=='0')
	{
		//printf("普通\n");
		work_meun(x);
	}else if(x.type=='4')
	{
		printf("重复登录或者密码错误\n");
		while(getchar()!=10);
	}

	return 0;
}
int work_meun(data tem)
{
	int select;
	
Q:
B:
C:
	while(1)
	{
		system("clear");
		printf("*********普通员工管理系统*********\n");
		printf("************1.增加****************\n");
		printf("************2.修改****************\n");
		printf("************3.查找****************\n");
		printf("************4.退出****************\n");
		printf("************V.177*****************\n");
		printf("input>>>>>");
		scanf("%d",&select);
		while(getchar()!=10);
		switch(select)
		{
		case 1:
			work_add(tem);
			goto Q;
			break;
		case 2:
			work_change(tem);
			goto B;
			break;
		case 3:
			work_find(tem);
			goto C;
			break;
		case 4:
			do_exit();
			return 0;
			break;
		default:
			printf("input error\n");
		}
		printf("输入任意字符刷新页面>>>>");
		while(getchar()!=10);
	}
	return 0;
}


int work_find(data tem)
{
	data x;                    
	size_t size=0;             
	char buf[sizeof(x)];   
	printf("input ID>>>>>");
	scanf("%s",x.data1.ID);
	while(getchar()!=10);
	x.type='d';
	if(strcmp(tem.data2.account,x.data1.ID)!=0)
	{
		x.type='c';
	}
	if(send(sockid,(void *)&x,sizeof(x),0)<0)                 
	{                                                         
		ERR_MSG("send");                                      
		return -1;                                            
	}                                                         
	size=recv(sockid,buf,sizeof(buf),0);//接收服务器的数据    
	if(size<0){                                               
		ERR_MSG("recv");                                      
		return -1;                                            
	} 
	x=*(data *)buf;   
	printf("%c\n",x.type);
	if(x.type=='0')
	{
		printf("name not find\n");
		while(getchar()!=10);
		goto B;	
	}
	else if(x.type=='c')
	{
		printf("只查看其他员工的部分信息\n");
		printf("name\tsex\tage\tID\tphone\n");
		printf("%s\t%c\t%s\t%s\t%s\n",x.data1.brr,x.data1.sex,x.data1.year,x.data1.ID,x.data1.phone);
		printf("查看成功\n输入任意字符进行下一步\n");
		while(getchar()!=10);
	}
	else if(x.type=='d')
	{
		printf("name\tsex\tage\taddr\tpay\tID\tphone\n");
		printf("%s\t%c\t%s\t%s\t%s\t%s\t%s\n",x.data1.brr,x.data1.sex,x.data1.year,x.data1.addr,x.data1.age,x.data1.ID,x.data1.phone);
		printf("查看成功\n输入任意字符进行下一步\n");
		while(getchar()!=10);
	}
B:
	return 0;
}

int work_change(data tem)
{
	data x;                    
	size_t size=0;             
	char buf[sizeof(x)];       
	x.type='a';
	printf("input ID>>>>>");
	scanf("%s",x.data1.ID);
	while(getchar()!=10);
	if(strcmp(tem.data2.account,x.data1.ID)!=0)
	{
		printf("员工只能修改自己的,请重新输入\n");
		while(getchar()!=10);
		goto A;
	}
	printf("input name>>>>>");
	scanf("%s",x.data1.brr);
	while(getchar()!=10);
	printf("input sex>>>>>");
	scanf("%c",&x.data1.sex);
	while(getchar()!=10);
	printf("input age>>>>>");
	scanf("%s",x.data1.year);
	while(getchar()!=10);
	printf("input addr>>>>>");
	scanf("%s",x.data1.addr);
	while(getchar()!=10);
	printf("input phone>>>>>");
	scanf("%s",x.data1.phone);
	while(getchar()!=10);
	if(send(sockid,(void *)&x,sizeof(x),0)<0)                 
	{                                                         
		ERR_MSG("send");                                      
		return -1;                                            
	}                                                         

	size=recv(sockid,buf,sizeof(buf),0);//接收服务器的数据    
	if(size<0){                                               
		ERR_MSG("recv");                                      
		return -1;                                            
	} 
	x=*(data *)buf;      
	if(x.type=='0')
	{
		printf("name is not\n");
		while(getchar()!=10);
		goto B;
	}
	printf("name\tsex\tage\taddr\tID\tphone\n");
	printf("%s\t%c\t%s\t%s\t%s\t%s\n",x.data1.brr,x.data1.sex,x.data1.year,x.data1.addr,x.data1.ID,x.data1.phone);
	printf("修改成功\n输入任意字符进行下一步\n");
	while(getchar()!=10);
A:
B:
	return 0;
}

int work_add(data tem)
{
	data x;
	size_t size=0;
	char buf[sizeof(x)];
	x.type='7';
	printf("input ID>>>>>");
	scanf("%s",x.data1.ID);
	while(getchar()!=10);
	if(strcmp(tem.data2.account,x.data1.ID)!=0)
	{
		printf("员工只能增加自己的,请重新输入\n");
		while(getchar()!=10);
		goto A;
	}
	printf("input name>>>>>");
	scanf("%s",x.data1.brr);
	while(getchar()!=10);
	printf("input sex>>>>>");
	scanf("%c",&x.data1.sex);
	while(getchar()!=10);
	printf("input age>>>>>");
	scanf("%s",x.data1.year);
	while(getchar()!=10);
	printf("input addr>>>>>");
	scanf("%s",x.data1.addr);
	while(getchar()!=10);
	printf("input pay>>>>>");
	scanf("%s",x.data1.age);
	while(getchar()!=10);
	printf("input phone>>>>>");
	scanf("%s",x.data1.phone);
	while(getchar()!=10);
	if(send(sockid,(void *)&x,sizeof(x),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}

	size=recv(sockid,buf,sizeof(buf),0);//接收服务器的数据
	if(size<0){
		ERR_MSG("recv");
		return -1;
	}
	x=*(data *)buf;
	if(x.type=='5')
	{
		printf("添加成功\n输入任意字符进行下一步\n");
		while(getchar()!=10);
	}
	else if(x.type=='6')
	{
		printf("已存在\n输入任意字符进行下一步\n");
		while(getchar()!=10);	
	}
A:
	return 0;
}
int system_meun(data tem)
{
	int select;
	while(1)
	{
		system("clear");
		printf("***********system管理系统*********\n");
		printf("************1.增加****************\n");
		printf("************2.修改****************\n");
		printf("************3.查找****************\n");
		printf("************4.删除****************\n");
		printf("************5.退出****************\n");
		printf("************V.177*****************\n");
		printf("input>>>>>");
		scanf("%d",&select);
		while(getchar()!=10);
		switch(select)
		{
		case 1:
			system_add();
			break;
		case 2:
			system_change();
			break;
		case 3:
			system_find();
			break;
		case 4:
			system_del();
			break;
		case 5:
			do_exit();
			return 0;
			break;
		default:
			printf("input error\n");
		}
		printf("输入任意字符刷新页面>>>>");
		while(getchar()!=10);
	}
	return 0;
}

int system_del()
{
	data x;                    
	size_t size=0;             
	char buf[sizeof(x)];
	printf("input ID>>>>>");
	scanf("%s",x.data1.ID);
	while(getchar()!=10);
	x.type=26;
	if(send(sockid,(void *)&x,sizeof(x),0)<0)                 
	{                                                         
		ERR_MSG("send");                                      
		return -1;                                            
	}                                                         
	size=recv(sockid,buf,sizeof(buf),0);//接收服务器的数据    
	if(size<0){                                               
		ERR_MSG("recv");                                      
		return -1;                                            
	} 
	x=*(data *)buf;      
	if(x.type=='0')
	{
		printf("name not find\n");
		while(getchar()!=10);
		goto B;	
	}
	if(x.type==26)
	{
		printf("已将%s的个人信息删除\n",x.data1.brr);
	}
B:
	return 0;
}
int system_find()
{	
	data x;                    
	size_t size=0;             
	char buf[sizeof(x)];
	printf("input ID>>>>>");
	scanf("%s",x.data1.ID);
	while(getchar()!=10);
	x.type=25;
	if(send(sockid,(void *)&x,sizeof(x),0)<0)                 
	{                                                         
		ERR_MSG("send");                                      
		return -1;                                            
	}                                                         
	size=recv(sockid,buf,sizeof(buf),0);//接收服务器的数据    
	if(size<0){                                               
		ERR_MSG("recv");                                      
		return -1;                                            
	} 
	x=*(data *)buf;      
	if(x.type=='0')
	{
		printf("name not find\n");
		while(getchar()!=10);
		goto B;	
	}
	if(x.type==25)
	{
		printf("name\tsex\tage\taddr\tpay\tID\tphone\n");
		printf("%s\t%c\t%s\t%s\t%s\t%s\t%s\n",x.data1.brr,x.data1.sex,x.data1.year,x.data1.addr,x.data1.age,x.data1.ID,x.data1.phone);
	}
B:
	return 0;
}
int system_change()
{
	data x;                    
	size_t size=0;             
	char buf[sizeof(x)];       
	x.type=23;
	printf("Please enter what the modify ID>>>>>");
	scanf("%s",x.data1.ID);
	while(getchar()!=10);
	printf("Please enter what the modify name>>>>>");
	scanf("%s",x.data1.brr);
	while(getchar()!=10);
	printf("Please enter what the modify sex>>>>>");
	scanf("%c",&x.data1.sex);
	while(getchar()!=10);
	printf("Please enter what the modify age>>>>>");
	scanf("%s",x.data1.year);
	while(getchar()!=10);
	printf("Please enter what the modify addr>>>>>");
	scanf("%s",x.data1.addr);
	while(getchar()!=10);
	printf("Please enter what the modify pay>>>>>");
	scanf("%s",x.data1.age);
	while(getchar()!=10);
	printf("Please enter what the modify phone>>>>>");
	scanf("%s",x.data1.phone);
	while(getchar()!=10);
	if(send(sockid,(void *)&x,sizeof(x),0)<0)                 
	{                                                         
		ERR_MSG("send");                                      
		return -1;                                            
	}                                                         

	size=recv(sockid,buf,sizeof(buf),0);//接收服务器的数据    
	if(size<0){                                               
		ERR_MSG("recv");                                      
		return -1;                                            
	} 
	x=*(data *)buf;      
	if(x.type==24)
	{
		printf("name not find\n");
		while(getchar()!=10);
		goto B;
	}
	printf("name\tsex\tage\taddr\tpay\tID\tphone\n");
	printf("%s\t%c\t%s\t%s\t%s\t%s\t%s\n",x.data1.brr,x.data1.sex,x.data1.year,x.data1.addr,x.data1.age,x.data1.ID,x.data1.phone);
B:
	return 0;
}

int system_add()
{
	data x;
	size_t size=0;
	char buf[sizeof(x)];
	x.type=20;
	printf("input ID>>>>>");
	scanf("%s",x.data1.ID);
	while(getchar()!=10);
	printf("input name>>>>>");
	scanf("%s",x.data1.brr);
	while(getchar()!=10);
	printf("input sex>>>>>");
	scanf("%c",&x.data1.sex);
	while(getchar()!=10);
	printf("input age>>>>>");
	scanf("%s",x.data1.year);
	while(getchar()!=10);
	printf("input addr>>>>>");
	scanf("%s",x.data1.addr);
	while(getchar()!=10);
	printf("input pay>>>>>");
	scanf("%s",x.data1.age);
	while(getchar()!=10);
	printf("input phone>>>>>");
	scanf("%s",x.data1.phone);
	while(getchar()!=10);
	if(send(sockid,(void *)&x,sizeof(x),0)<0)
	{
		ERR_MSG("send");
		return -1;
	}
	size=recv(sockid,buf,sizeof(buf),0);//接收服务器的数据
	if(size<0){
		ERR_MSG("recv");
		return -1;
	}
	x=*(data *)buf;
	if(x.type==21)
	{
		printf("添加成功\n");
	}
	else if(x.type==22)
	{
		printf("已存在\n");
	}
	return 0;
}

