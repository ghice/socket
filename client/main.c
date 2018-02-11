#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment(lib , "ws2_32.lib")

#define BUFSIZE 4096 /*缓冲区大小*/

int main(int argc, char *argv[])
{
	WSADATA wsd;
	SOCKET sClient;
	char Buffer[BUFSIZE];
	int ret,n;
	struct sockaddr_in server;
	unsigned short port;
	struct hostent *host = NULL;

	if (argc < 3) {
		printf("Usage:%s IP Port\n", argv[0]);
		return -1;
	}


	/*加载Winsock DLL*/
	if (WSAStartup(MAKEWORD(2, 2), &wsd) != 0) {
		printf("Winsock    初始化失败!\n");
		return 1;
	}

	/*创建Socket*/
	sClient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sClient == INVALID_SOCKET) {
		printf("socket() 失败: %d\n", WSAGetLastError());
		return 1;
	}
	/*指定服务器地址*/
	server.sin_family = AF_INET;
	port = atoi(argv[2]);
	server.sin_port = htons(port);
	server.sin_addr.s_addr = inet_addr(argv[1]);

	if (server.sin_addr.s_addr == INADDR_NONE) {
		host = gethostbyname(argv[1]);    //输入的地址可能是域名等
		if (host == NULL) {
			printf("无法解析服务端地址: %s\n", argv[1]);
			return 1;
		}
		CopyMemory(&server.sin_addr,
			host->h_addr_list[0],
			host->h_length);
	}
	/*与服务器建立连接*/
	if (connect(sClient, (struct sockaddr*)&server,
		sizeof(server)) == SOCKET_ERROR) {
		printf("connect() 失败: %d\n", WSAGetLastError());
		return 1;
	}

	/*发送、接收消息*/

	for (;;) {
		//从标准输入读取要发送的数据
		//gets_s(Buffer,BUFSIZE);
		printf("请输入上下行：(上行（1）/下行（2））和楼层（1~7）\n例如：17（地表上到7楼）:");
		scanf("%s", Buffer);
		Buffer[2] = '\0';
		//向服务器发送消息
		ret = send(sClient, Buffer, strlen(Buffer), 0);
		if (ret == 0) {
			break;
		}
		else if (ret == SOCKET_ERROR) {
			printf("send() 失败: %d\n", WSAGetLastError());
			break;
		}
		printf("Send %d bytes\n", ret);
	
	}
	//用完了，关闭socket句柄(文件描述符)
	closesocket(sClient);
	WSACleanup();    //清理
	return 0;
}

