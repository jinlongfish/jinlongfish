#include<stdio.h>
#include<string.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")

//DWORD (WINAPI *PTHREAD_START_ROUTINE)(LPVOID lpThreadParameter);
DWORD WINAPI thread_func(LPVOID lpThreadParameter)
{
	SOCKET client_socket = *(SOCKET*)lpThreadParameter;
	free(lpThreadParameter);
		while(1)
		{
			char buffer[1024] = {0};
			int ret = recv(client_socket,buffer,1024,0);//接收数据
			if(ret <= 0)break;
			printf("%d:%s\n",client_socket,buffer);

			send(client_socket,buffer,(int)strlen(buffer),0);
		}
		printf("socket:%d -> disconnect.\n",client_socket);
		//关闭连接
		closesocket(client_socket);
	return 0;
}


int main()
{
	//仅限windows,启动服务（其实不是，我也不知道）
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	//创建socket套接字
	/*SOCKET socket(
		 int af,          //协议地址簇IPv4/IPv6 -> AF_INET/AF_INET6
		 int type,        //类型 流式协议/帧式协议 -> SOCK_STREAM/SOCK_DGRAM
		 int protocol	  //保护协议 暂时不用
		);*/
	SOCKET listen_socket = socket(AF_INET,SOCK_STREAM,0);
	if(listen_socket == INVALID_SOCKET)
	{
		printf("creat listen socket filed!  errorcode:%d\n",GetLastError());
		return -1;
	}
	//给socket绑定端口
	/*
	struct sockaddr_in {

		ADDRESS_FAMILY sin_family;    //协议地址簇
		USHORT sin_port;			  //端口号
		IN_ADDR sin_addr;			  //IP地址
		CHAR sin_zero[8];			  //保留字节

	}
*/
	struct sockaddr_in local = {0};
	local.sin_family = AF_INET;
	local.sin_port = htons(8080);				//htons转换，因为像路由器这样的中间设备使用的是大端序号（1F 90），而我们电脑上则是用的是小端序号（90 1F）,不转换的话，端口号不同无法接通
	local.sin_addr.s_addr = inet_addr("0.0.0.0");  //htonl(INADDR_ANY)//同样有大小端问题，需要转换，这是服务端选项，指定只接受哪个ip的访问，127.0.0.1（自己访问自己，本地环回）/0.0.0.0（接受任何ip访问）//手动添加要inet_addr("your ip");

/*	int bind(
    SOCKET s,
    const struct sockaddr * name,
    int namelen
    );
*/


	if(bind(listen_socket,(struct sockaddr*)&local,sizeof(local)) == -1)
	{
		printf("bind listen socket filed!  errorcode:%d\n",GetLastError());
		return -1;
	}

	//给socket开启监听属性

/*	int listen(
    SOCKET s,
    int backlog  //长度（一般10就够了）
    );
*/
	if(listen(listen_socket,10) == -1)
	{
		printf("start listen socket filed!  errorcode:%d\n",GetLastError());
		return -1;
	}
///////////////////////////////////////////////////////////通讯部分
	//等待客户端连接
/*	SOCKET accept(
		SOCKET s,					//监听socket
		struct sockaddr * addr,		//返回客户端ip和端口号（与下边关联）
		int * addrlen				//结构大小（若填空（NULL），上面的就不用填）
    );
*/
	//accept是阻塞函数，当有客户端连接进来就接受连接

	while(1)
	{
		SOCKET client_socket = accept(listen_socket,NULL,NULL);//client_socket是通信socket，发消息发到它上面
		if(INVALID_SOCKET == client_socket)
			continue;
		printf("new socket connect:%d\n",client_socket);
		//开始通讯
		SOCKET* sockfd = (SOCKET*)malloc(sizeof(SOCKET));
		*sockfd = client_socket;
		//创建线程
		CreateThread(NULL,0,thread_func,sockfd,0,NULL);


	}

	return 0;
}




/*		int recv(
			SOCKET s,     //客户端socket
			char * buf,	 //接收的数据存到哪里
			int len,		 //接收的长度
			int flags	 //标记
				);
*/