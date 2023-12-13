#include "socket.h"

#define REMOTEPORT 9000
#define BUFSIZE    512

void newUser(struct sockaddr_in clinet);
void pushAllUser(char buf[], SOCKET* sock);
struct sockaddr_in nextUser();
int exitUser(int port);
int getUserList(char buf[]);

// 랜덤 단어
char randSpell[10][10] = { "물고기", "기차", "하늘", "눈사람", "하늘", "컴퓨터", "음악", "도서관", "여행", "책상"};

int main(int argc, char* argv[])
{
	int result;

	srand(time(NULL)); //랜덤 초기화

	// 윈속 초기화
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// 소켓 생성
	SOCKET sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == INVALID_SOCKET) err_quit("socket()");

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
	struct sockaddr_in clnt_addr;
	int len;

	while (1) {
		// 데이터 받기
		len = sizeof(clnt_addr);
		result = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &len);
		if (result == SOCKET_ERROR) {
			err_display("recvfrom()");
			break;
		}

		// 받은 데이터 출력
		buf[result] = '\0';
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clnt_addr.sin_addr, addr, sizeof(addr));
		printf("[%s:%d] %s\n", addr, ntohs(clnt_addr.sin_port), buf);

		//문자에 따른 명령처리
		if (strstr(buf, "connect")) {
			char conbuf[BUFSIZE]; //접속한 클라이언트 전용 버퍼
			char listbuf[BUFSIZE]; //유저 목록 버퍼

			sprintf(buf, "connect-%d", ntohs(clnt_addr.sin_port));
			sprintf(conbuf, "conUserid-%d", ntohs(clnt_addr.sin_port));

			// 클라이언트가 자기 포트를 식별할수 있도록 따로 리턴
			result = sendto(sock, conbuf, (int)strlen(conbuf), 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
			if (result == SOCKET_ERROR) {
				err_display("sendto()");
				break;
			}

			// 유저목록 조회 및 리턴
			if (getUserList(conbuf)) {
				sprintf(listbuf, "user-%s", conbuf);
			} else {
				int randint = rand() % 9;
				sprintf(listbuf, "next-%s-%d", randSpell[randint], ntohs(clnt_addr.sin_port));
			}

			result = sendto(sock, listbuf, (int)strlen(listbuf), 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
			if (result == SOCKET_ERROR) {
				err_display("sendto()");
				break;
			}

			// 모든 유저에게 메시지 보내기
			pushAllUser(buf, &sock);

			//연결리스트에 유저 추가
			newUser(clnt_addr);
		} else if (strstr(buf, "spelling")) {
			char* data = NULL; //단어만 저장해놓는곳
			char* spell = strtok_s(buf, "-", &data);

			struct sockaddr_in next = nextUser();

			// next-단어-입력할유저포트
			sprintf(buf, "next-%s-%d", data, ntohs(next.sin_port));

			// 모든 유저에게 메시지 보내기
			pushAllUser(buf, &sock);
		} else if (strstr(buf, "exit")) {
			char* data = NULL;	//포트번호만 저장해 놓는곳
			char* exit = strtok_s(buf, "-", &data);

			sprintf(buf, "exit-%s", data);
			// 모든 유저에게 메시지 보내기
			pushAllUser(buf, &sock);

			// 만일 나간 유저가 다음 차례인 경우
			if (exitUser(atoi(data))){
				struct sockaddr_in next = nextUser();

				int randint = rand() % 9;
				// next-단어-입력할유저포트
				sprintf(buf, "next-%s-%d", randSpell[randint], ntohs(next.sin_port));

				// 모든 유저에게 메시지 보내기
				pushAllUser(buf, &sock);
			}
		}
	}

	// 소켓 닫기
	closesocket(sock);

	// 윈속 종료
	WSACleanup();
	return 0;
}
