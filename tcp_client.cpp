#include<stdio.h>
#include<WinSock2.h>
#pragma comment(lib,"ws2_32.lib")
int main()
{	
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);

	//创建socket套接字
	SOCKET client_socket = socket(AF_INET,SOCK_STREAM,0);
	if(client_socket == INVALID_SOCKET)
	{
		printf("create client socket filed!\n");
		return -1;
	}

	//连接服务器
	struct sockaddr_in target;
	target.sin_family = AF_INET;
	target.sin_port = htons(8080);
	target.sin_addr.s_addr = inet_addr("124.222.54.249");

	if(connect(client_socket,(struct sockaddr*)&target,sizeof(target)) == -1)
	{
		printf("connect server filed!\n");
		closesocket(client_socket);
		return -1;
	}

	//开始通讯 send recv
	while(1)
	{
		char sbuffer[1024] = {0};
		printf("请输入：");
		scanf("%s",&sbuffer);
		send(client_socket,sbuffer,strlen(sbuffer),0);

		char rbuffer[1024] = {0};
		int ret = recv(client_socket,rbuffer,1024,0);
		if(ret<=0)break;
		printf("%s\n",rbuffer);
	}
	//关闭socket
	closesocket(client_socket);
	return 0;
}
