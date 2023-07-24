#include<iostream>
#pragma warning(disable:4996)
#include<WS2tcpip.h>
#include<Windows.h>
#include<winsock2.h>
#pragma comment(lib,"ws2_32.lib")
using namespace std;

const int MaxNum = 100;

SOCKET ClntSocks[MaxNum];//��ſͻ��˵�socket��externȫ�ֱ���

int CountNum = 0;//ͳ�Ƶ�ǰ�Ŀͻ�������

SOCKET ClntSock;

void HandleClnt(int i) {
	char buff[256] = { 0 };
	char temp[256] = { 0 };
	int Strlen;
	while (true) {
		Strlen = recv(ClntSocks[i], buff, 255, NULL);

		if (Strlen > 0) {
			cout << i << ":" << buff << endl;

			//�������пͻ���
			memset(temp, 0, 256);//���
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
		std::cout << "ȷ��Э��汾ʧ�ܣ�" << GetLastError() << std::endl;
		return -1;
	}
	std::cout << "ȷ��Э��汾�ɹ�" << std::endl;


	//���������׽���
	SOCKET ServSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);//����TCP�׽���
	if (ServSock == SOCKET_ERROR) {
		cout << "�����׽��ִ���ʧ��" << endl;
		return -2;
	}
	cout << "�����׽��ִ����ɹ�" << endl;

	SOCKADDR_IN sock_in = { 0 };
	sock_in.sin_family = AF_INET;
	sock_in.sin_port = htons(10086);
	sock_in.sin_addr.S_un.S_addr = inet_addr("192.168.181.1");

	int ret = bind(ServSock, (sockaddr*)&sock_in, sizeof sock_in);//��(ip �� socket��)

	if (ret == SOCKET_ERROR) {
		cout << "���׽���ʧ��" << endl;
		closesocket(ServSock);
		WSACleanup();
		return -1;
	}
	cout << "���׽��ֳɹ�" << endl;


	if (listen(ServSock, 10) == SOCKET_ERROR) {//�������˴��ڶ���������˵ �ں�Ϊ ���׽���ά����������Ӹ���
		cout << "�����׽���ʧ��" << endl;
		closesocket(ServSock);
		WSACleanup();
		return -2;
	}
	cout << "�����׽��ֳɹ�" << endl;

	for (int i = 0; i < MaxNum; i++) {
		//���ӿͻ���
		ClntSock = accept(ServSock, NULL, NULL);


		if (ClntSock == SOCKET_ERROR) {
			cout << "���տͻ���ʧ��" << GetLastError() << endl;
			closesocket(ServSock);
			WSACleanup();
			return -3;
		}
		cout << "���ӿͻ��˳ɹ�" << endl;

		//���ͻ��˵�socket��������
		ClntSocks[i] = ClntSock;

		CountNum++;

		//Ϊ�ͻ��˷����̣߳����̸߳��������Ϣ
		CreateThread(0, 0, (LPTHREAD_START_ROUTINE)HandleClnt, (LPVOID)i, 0, 0);

	}

	while (true);
	return 0;
}