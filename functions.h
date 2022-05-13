#pragma once

#include <ctime> 
void gettime(char currenttime[50]) {
	auto end = std::chrono::system_clock::now();
	std::time_t end_time = std::chrono::system_clock::to_time_t(end);
	ctime_s(currenttime, 50, &end_time);
}


void split(char buff[15 + 1 + 15], char name[15], char password[15]) {
	int count = 0, i, j = 0;
	for (i = 0; i < strlen(buff); i++) {
		if (buff[i] == '_') {
			switch (count) {
			case 0:
				name[j] = '\0';
				break;
			case 1:
				password[j] = '\0';
				break;
			}
			j = 0;
			count++;
			continue;
		}
		switch (count) {
		case 0:
			name[j] = buff[i];
			break;
		case 1:
			password[j] = buff[i];
			break;
		}
		j++;
	}
	if (count == 1)
		password[j] = '\0';
	//cout << "buffer: " << buff << " name: " << name << " password: " << password << endl;
}

void split(char buff[15 + 1 + 15 + 1 + DEFAULT_BUFLEN + 1 + 4], char toname[15], char fromname[15], char message[DEFAULT_BUFLEN], int* num) {
	int count = 0, i, j = 0;
	char send_not[4];
	for (i = 0; i < strlen(buff); i++) {
		if (buff[i] == '_') {
			switch (count) {
			case 0:
				toname[j] = '\0';
				break;
			case 1:
				fromname[j] = '\0';
				break;
			case 2:
				message[j] = '\0';
				break;
			case 3:
				send_not[j] = '\0';
				break;
			}
			j = 0;
			count++;
			continue;
		}
		switch (count) {
		case 0:
			toname[j] = buff[i];
			break;
		case 1:
			fromname[j] = buff[i];
			break;
		case 2:
			message[j] = buff[i];
			break;
		case 3:
			send_not[j] = buff[i];
			break;
		}
		j++;
	}
	if (count == 3)
		send_not[j] = '\0';
	if (strcmp("sen", send_not) == 0) { 
		// sent
		*num = 1;
	}
	else if (strcmp("not", send_not) == 0) { 
		// not
		*num = 0;
	}
	else { 
		// see
		*num = 2;
	}
}

void replaceChar(char s[DEFAULT_BUFLEN], char oldchar, char newchar) {
	int i;
	for (i = 0; i < strlen(s); i++) {
		if (s[i] == oldchar) {
			s[i] = newchar;
		}
	}
	s[i] = '\0';
}