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
			int ret = recv(client_socket,buffer,1024,0);//��������
			if(ret <= 0)break;
			printf("%d:%s\n",client_socket,buffer);

			send(client_socket,buffer,(int)strlen(buffer),0);
		}
		printf("socket:%d -> disconnect.\n",client_socket);
		//�ر�����
		closesocket(client_socket);
	return 0;
}


int main()
{
	//����windows,����������ʵ���ǣ���Ҳ��֪����
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2,2),&wsaData);
	//����socket�׽���
	/*SOCKET socket(
		 int af,          //Э���ַ��IPv4/IPv6 -> AF_INET/AF_INET6
		 int type,        //���� ��ʽЭ��/֡ʽЭ�� -> SOCK_STREAM/SOCK_DGRAM
		 int protocol	  //����Э�� ��ʱ����
		);*/
	SOCKET listen_socket = socket(AF_INET,SOCK_STREAM,0);
	if(listen_socket == INVALID_SOCKET)
	{
		printf("creat listen socket filed!  errorcode:%d\n",GetLastError());
		return -1;
	}
	//��socket�󶨶˿�
	/*
	struct sockaddr_in {

		ADDRESS_FAMILY sin_family;    //Э���ַ��
		USHORT sin_port;			  //�˿ں�
		IN_ADDR sin_addr;			  //IP��ַ
		CHAR sin_zero[8];			  //�����ֽ�

	}
*/
	struct sockaddr_in local = {0};
	local.sin_family = AF_INET;
	local.sin_port = htons(8080);				//htonsת������Ϊ��·�����������м��豸ʹ�õ��Ǵ����ţ�1F 90���������ǵ����������õ���С����ţ�90 1F��,��ת���Ļ����˿ںŲ�ͬ�޷���ͨ
	local.sin_addr.s_addr = inet_addr("0.0.0.0");  //htonl(INADDR_ANY)//ͬ���д�С�����⣬��Ҫת�������Ƿ����ѡ�ָ��ֻ�����ĸ�ip�ķ��ʣ�127.0.0.1���Լ������Լ������ػ��أ�/0.0.0.0�������κ�ip���ʣ�//�ֶ����Ҫinet_addr("your ip");

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

	//��socket������������

/*	int listen(
    SOCKET s,
    int backlog  //���ȣ�һ��10�͹��ˣ�
    );
*/
	if(listen(listen_socket,10) == -1)
	{
		printf("start listen socket filed!  errorcode:%d\n",GetLastError());
		return -1;
	}
///////////////////////////////////////////////////////////ͨѶ����
	//�ȴ��ͻ�������
/*	SOCKET accept(
		SOCKET s,					//����socket
		struct sockaddr * addr,		//���ؿͻ���ip�Ͷ˿ںţ����±߹�����
		int * addrlen				//�ṹ��С������գ�NULL��������ľͲ����
    );
*/
	//accept���������������пͻ������ӽ����ͽ�������

	while(1)
	{
		SOCKET client_socket = accept(listen_socket,NULL,NULL);//client_socket��ͨ��socket������Ϣ����������
		if(INVALID_SOCKET == client_socket)
			continue;
		printf("new socket connect:%d\n",client_socket);
		//��ʼͨѶ
		SOCKET* sockfd = (SOCKET*)malloc(sizeof(SOCKET));
		*sockfd = client_socket;
		//�����߳�
		CreateThread(NULL,0,thread_func,sockfd,0,NULL);


	}

	return 0;
}




/*		int recv(
			SOCKET s,     //�ͻ���socket
			char * buf,	 //���յ����ݴ浽����
			int len,		 //���յĳ���
			int flags	 //���
				);
*/