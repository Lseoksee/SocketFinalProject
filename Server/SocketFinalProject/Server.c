#include "socket.h"

#define REMOTEPORT 9000
#define BUFSIZE    512

void newUser(struct sockaddr_in clinet);
void pushAllUser(char buf[], SOCKET* sock);
struct sockaddr_in nextUser();
int exitUser(int port);
int getUserList(char buf[]);

// ���� �ܾ�
char randSpell[10][10] = { "�����", "����", "�ϴ�", "�����", "�ϴ�", "��ǻ��", "����", "������", "����", "å��"};

int main(int argc, char* argv[])
{
	int result;

	srand(time(NULL)); //���� �ʱ�ȭ

	// ���� �ʱ�ȭ
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		return 1;

	// ���� ����
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

	// ������ ��ſ� ����� ����
	char buf[BUFSIZE + 1];
	struct sockaddr_in clnt_addr;
	int len;

	while (1) {
		// ������ �ޱ�
		len = sizeof(clnt_addr);
		result = recvfrom(sock, buf, BUFSIZE, 0, (struct sockaddr*)&clnt_addr, &len);
		if (result == SOCKET_ERROR) {
			err_display("recvfrom()");
			break;
		}

		// ���� ������ ���
		buf[result] = '\0';
		char addr[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &clnt_addr.sin_addr, addr, sizeof(addr));
		printf("[%s:%d] %s\n", addr, ntohs(clnt_addr.sin_port), buf);

		//���ڿ� ���� ���ó��
		if (strstr(buf, "connect")) {
			char conbuf[BUFSIZE]; //������ Ŭ���̾�Ʈ ���� ����
			char listbuf[BUFSIZE]; //���� ��� ����

			sprintf(buf, "connect-%d", ntohs(clnt_addr.sin_port));
			sprintf(conbuf, "conUserid-%d", ntohs(clnt_addr.sin_port));

			// Ŭ���̾�Ʈ�� �ڱ� ��Ʈ�� �ĺ��Ҽ� �ֵ��� ���� ����
			result = sendto(sock, conbuf, (int)strlen(conbuf), 0, (struct sockaddr*)&clnt_addr, sizeof(clnt_addr));
			if (result == SOCKET_ERROR) {
				err_display("sendto()");
				break;
			}

			// ������� ��ȸ �� ����
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

			// ��� �������� �޽��� ������
			pushAllUser(buf, &sock);

			//���Ḯ��Ʈ�� ���� �߰�
			newUser(clnt_addr);
		} else if (strstr(buf, "spelling")) {
			char* data = NULL; //�ܾ �����س��°�
			char* spell = strtok_s(buf, "-", &data);

			struct sockaddr_in next = nextUser();

			// next-�ܾ�-�Է���������Ʈ
			sprintf(buf, "next-%s-%d", data, ntohs(next.sin_port));

			// ��� �������� �޽��� ������
			pushAllUser(buf, &sock);
		} else if (strstr(buf, "exit")) {
			char* data = NULL;	//��Ʈ��ȣ�� ������ ���°�
			char* exit = strtok_s(buf, "-", &data);

			sprintf(buf, "exit-%s", data);
			// ��� �������� �޽��� ������
			pushAllUser(buf, &sock);

			// ���� ���� ������ ���� ������ ���
			if (exitUser(atoi(data))){
				struct sockaddr_in next = nextUser();

				int randint = rand() % 9;
				// next-�ܾ�-�Է���������Ʈ
				sprintf(buf, "next-%s-%d", randSpell[randint], ntohs(next.sin_port));

				// ��� �������� �޽��� ������
				pushAllUser(buf, &sock);
			}
		}
	}

	// ���� �ݱ�
	closesocket(sock);

	// ���� ����
	WSACleanup();
	return 0;
}
