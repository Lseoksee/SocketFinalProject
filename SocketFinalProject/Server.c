#include "socket.h"

#define REMOTEIP   "255.255.255.255"
#define REMOTEPORT 9000
#define BUFSIZE    512

int main(int argc, char* argv[])
{
	int result;

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

	// 브로드캐스팅 활성화
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

	// 데이터 통신에 사용할 변수
	char buf[BUFSIZE + 1];
	int len;

	// 브로드캐스트 데이터 보내기
	while (1) {
		// 데이터 받기
		len = sizeof(localaddr);
		result = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&localaddr, &len);
		if (result == SOCKET_ERROR) {
			err_display("recvfrom()");
			break;
		}

		// 받은 데이터 출력
		buf[result] = '\0';
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &localaddr.sin_addr, addr, sizeof(addr));
		printf("[%s:%d] %s\n", addr, ntohs(localaddr.sin_port), buf);

		// 데이터 보내기
		result = sendto(sock, buf, (int)strlen(buf), 0,
			(struct sockaddr*)&localaddr, sizeof(localaddr));
		if (result == SOCKET_ERROR) {
			err_display("sendto()");
			break;
		}
	}

	// 소켓 닫기
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
