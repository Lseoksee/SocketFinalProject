#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <WinSock2.h>

typedef struct ListData {
	struct sockaddr_in clinet;
	struct ListData* next;
} ListData;

ListData* head = NULL;
ListData* nextTemp = NULL;

// ���ο� ���� ���ӽ�
void newUser(struct sockaddr_in clinet) {
	ListData* user = (ListData*) malloc(sizeof(ListData));
	user->next = NULL;
	user->clinet = clinet;

	if (head == NULL) {
		head = user;
		nextTemp = user;
		user->next = user;
		nextTemp->next = user;
	} else {
		user->next = head->next;
		head->next = user;
		head = user;
	}
}

// �ܾ��Է��ϰ� ���� ��������
struct sockaddr_in nextUser() {
	nextTemp = nextTemp->next;
	return nextTemp->clinet;
}

// �����
int exitUser(int port) {
	ListData* p = head;
	int exitCode = 0;
	while (p != NULL) {
		int target = ntohs(p->next->clinet.sin_port);
		if (port == target) {
			if (p == p->next) {
				head = NULL;
				nextTemp = NULL;
			}
			if (p->next == head) {
				head = p;
			}
			if (p->next == nextTemp) {
				exitCode = 1;
				nextTemp = p;
			}

			ListData* del = p->next;
			p->next = p->next->next;
			free(del);
			
			printf("������: %d\n", port);
			return exitCode;
		}

		p = p->next;
		if (p == head) {
			return exitCode;
		}
	}
}

// ��� �������� �޽��� Ǫ��
void pushAllUser(char buf[], SOCKET* sock) {
	ListData* p = head;
	int result;
	while (p != NULL) {

		// ������ ������
		result = sendto(*sock, buf, (int)strlen(buf), 0, (struct sockaddr*)&(p->clinet), sizeof(p->clinet));
		if (result == SOCKET_ERROR) {
			err_display("sendto()");
			break;
		}

		p = p->next;
		if (p == head) {
			break;
		}
	}
}

// ���� ��� ����
int getUserList(char buf[]) {
	ListData* p = head;
	char temp[512];
	buf[0] = '\0';	//���� ����

	while (p != NULL) {
		sprintf(temp, "%d,", ntohs(p->clinet.sin_port));
		strcat(buf, temp);

		p = p->next;
		if (p == head) {
			// ������ , �����
			buf[strlen(buf) - 1] = '\0';
			return 1;
		}
	}

	return 0;
}