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

// 새로운 유저 접속시
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

// 단어입력하고 다음 유저리턴
struct sockaddr_in nextUser() {
	nextTemp = nextTemp->next;
	return nextTemp->clinet;
}

// 종료시
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
			
			printf("삭제됨: %d\n", port);
			return exitCode;
		}

		p = p->next;
		if (p == head) {
			return exitCode;
		}
	}
}

// 모든 유저에게 메시지 푸시
void pushAllUser(char buf[], SOCKET* sock) {
	ListData* p = head;
	int result;
	while (p != NULL) {

		// 데이터 보내기
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

// 유저 목록 리턴
int getUserList(char buf[]) {
	ListData* p = head;
	char temp[512];
	buf[0] = '\0';	//버퍼 비우기

	while (p != NULL) {
		sprintf(temp, "%d,", ntohs(p->clinet.sin_port));
		strcat(buf, temp);

		p = p->next;
		if (p == head) {
			// 마지막 , 지우기
			buf[strlen(buf) - 1] = '\0';
			return 1;
		}
	}

	return 0;
}