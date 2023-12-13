#include "socket.h"

#define REMOTEIP   "255.255.255.255"
#define REMOTEPORT 9000
#define BUFSIZE    512

int main(int argc, char* argv[])
{
	int result;

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// ��ε�ĳ���� Ȱ��ȭ
	DWORD bEnable = 1;
	result = setsockopt(sock, SOL_SOCKET, SO_BROADCAST,
		(const char*)&bEnable, sizeof(bEnable));
	if (result == SOCKET_ERROR) err_quit("setsockopt()");

	// bind()
	struct sockaddr_in localaddr;
	memset(&localaddr, 0, sizeof(localaddr));
	localaddr.sin_family = AF_INET;
	localaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localaddr.sin_port = htons(REMOTEPORT);
	result = bind(sock, (struct sockaddr*)&localaddr, sizeof(localaddr));
	if (result == SOCKET_ERROR) err_quit("bind()");

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE + 1];
	int len;

	// ��ε�ĳ��Ʈ ������ ������
	while (1) {
		// ������ �ޱ�
		len = sizeof(localaddr);
		result = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&localaddr, &len);
		if (result == SOCKET_ERROR) {
			err_display("recvfrom()");
			break;
		}

		// ���� ������ ���
		buf[result] = '\0';
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &localaddr.sin_addr, addr, sizeof(addr));
		printf("[%s:%d] %s\n", addr, ntohs(localaddr.sin_port), buf);

		// ������ ������
		result = sendto(sock, buf, (int)strlen(buf), 0,
			(struct sockaddr*)&localaddr, sizeof(localaddr));
		if (result == SOCKET_ERROR) {
			err_display("sendto()");
			break;
		}
	}

	// ���� �ݱ�
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}
