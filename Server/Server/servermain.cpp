#include<iostream>
#pragma warning(disable:4996)
#include<WS2tcpip.h>
#include<Windows.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

const int MaxNum = 100;

SOCKET ClntSocks[MaxNum];//存放客户端的socket，extern全局变量

int CountNum = 0;//统计当前的客户端数量

SOCKET ClntSock;

void HandleClnt(int i) {
	char buff[256] = { 0 };
	char temp[256] = { 0 };
	int Strlen;
	while (true) {
		Strlen = recv(ClntSocks[i], buff, 255, NULL);

		if (Strlen > 0) {
			cout << i << ":" << buff << endl;

			//发给所有客户端
			memset(temp, 0, 256);//清空
			sprintf_s(temp, "%d:%s", i, buff);
			for (int j = 0; j < CountNum; j++) {
				send(ClntSocks[j], temp, strlen(temp), NULL);
			}
		}
		else {
			cout << Strlen << endl;
		}
	}
}


int main() {

	WSADATA wsData;
	WSAStartup(MAKEWORD(2, 2), &wsData);

	if (LOBYTE(wsData.wVersion) != 2 || HIBYTE(wsData.wVersion) != 2) {
		std::cout << "确定协议版本失败：" << GetLastError() << std::endl;
		return -1;
	}
	std::cout << "确定协议版本成功" << std::endl;


	//创建监听套接字
	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建TCP套接字
	if (ServSock == SOCKET_ERROR) {
		cout << "监听套接字创建失败" << endl;
		return -2;
	}
	cout << "监听套接字创建成功" << endl;

	SOCKADDR_IN sock_in = { 0 };
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons(10086);
	sock_in.sin_addr.S_un.S_addr = inet_addr("192.168.181.1");

	int ret = bind(ServSock, (sockaddr*)&sock_in, sizeof sock_in);//绑定(ip 与 socket绑定)

	if (ret == SOCKET_ERROR) {
		cout << "绑定套接字失败" << endl;
		closesocket(ServSock);
		WSACleanup();
		return -1;
	}
	cout << "绑定套接字成功" << endl;


	if (listen(ServSock, 10) == SOCKET_ERROR) {//监听，此处第二个参数是说 内核为 此套接字维护的最大链接个数
		cout << "监听套接字失败" << endl;
		closesocket(ServSock);
		WSACleanup();
		return -2;
	}
	cout << "监听套接字成功" << endl;

	for (int i = 0; i < MaxNum; i++) {
		//连接客户端
		ClntSock = accept(ServSock, NULL, NULL);


		if (ClntSock == SOCKET_ERROR) {
			cout << "接收客户端失败" << GetLastError() << endl;
			closesocket(ServSock);
			WSACleanup();
			return -3;
		}
		cout << "连接客户端成功" << endl;

		//将客户端的socket放入服务端
		ClntSocks[i] = ClntSock;

		CountNum++;

		//为客户端分配线程，该线程负责接收消息
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HandleClnt, (LPVOID)i, 0, 0);

	}

	while (true);
	return 0;
}