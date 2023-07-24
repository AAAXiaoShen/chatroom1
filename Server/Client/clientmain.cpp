#include<iostream>
#pragma warning(disable:4996)
#include<WS2tcpip.h>
#include<Windows.h>
#include<winsock2.h>
#include<graphics.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

void Send();

SOCKET ClntSock;

int main() {
	initgraph(200, 400 ,1);


	WSADATA wsData;
	WSAStartup(MAKEWORD(2, 2), &wsData);

	if (LOBYTE(wsData.wVersion) != 2 || HIBYTE(wsData.wVersion) != 2) {
		std::cout << "确定协议版本失败：" << GetLastError() << std::endl;
		return -1;
	}
	std::cout << "确定协议版本成功" << std::endl;


	//创建监听套接字
	ClntSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//创建TCP套接字
	if (ClntSock == SOCKET_ERROR) {
		cout << "监听套接字创建失败" << endl;
		return -2;
	}
	cout << "监听套接字创建成功" << endl;

	SOCKADDR_IN sock_in = { 0 };
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons(10086);
	sock_in.sin_addr.S_un.S_addr = inet_addr("192.168.181.1");

	if (connect(ClntSock, (sockaddr*)&sock_in, sizeof(sockaddr_in)) == SOCKET_ERROR) {
		cout << "连接服务器失败" << GetLastError() << endl;
		return 0;
	}

	cout << "与服务端建立连接" << endl;

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)Send, 0, 0, 0);

	char buff[256];

	int Strlen;

	int n = 0;

	initgraph(300, 400, 1);//接收消息的窗口

	while (true) {
		Strlen = recv(ClntSock, buff, 255, NULL);
		if (Strlen > 0) {
			buff[Strlen] = 0;
			//显示到图形界面窗口
			outtextxy(1, n * 20, buff);
			n++;
			if (n > 18) {
				n = 0;
				cleardevice();
			}
		}
	}

	while (true);
	return 0;
}


void Send() {
	char buff[256];
	while (true) {
		memset(buff, 0, 256);//清空数组
		cout << "请输入：" << endl;
		cin >> buff;
		send(ClntSock, buff, strlen(buff), NULL);//发信息给服务器
	}

}
