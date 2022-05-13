#pragma once

// Commands for Client
const int command_len = 10;
const int command_set = 4;
char commands[command_len][20] = {
	"name",		//0		//name_Client-1_hello world
	"names",	//1		//names_Client-1,Client-2,Client-3_hello world
	"signup",	//2		//signup_name_passsword
	"login",	//3		//login_name_passsword
	// single line set
	"logout",	//4		//logout
	"delete",	//5		//delete
	"remove",	//6		//remove
	"myname",	//7		//myname
	"name",		//8		//name
	"others",   //9		//others
};

void split(char word[10][15], char sentence[DEFAULT_BUFLEN], int* num) {
	int i, j = 0, count = 0;
	for (i = 0; i < strlen(sentence); i++) {
		if (sentence[i] == ',') {
			word[count][j] = '\0';
			count++;
			j = 0;
			continue;
		}
		word[count][j] = sentence[i];
		j++;
	}
	word[count][j] = '\0';
	*num = count + 1;
}



// num = 0 (Only splitting) (Used to decode from login.txt)
// num = 1 (splitting and analysing) (Used to decode from clients input message)
void analyse(char buff[DEFAULT_BUFLEN], char word0[DEFAULT_BUFLEN], char word1[DEFAULT_BUFLEN], char word2[DEFAULT_BUFLEN], int* num) {
	int count = 0, j = 0, i;
	for (i = 0; i < strlen(buff); i++) {
		if (buff[i] == '_') {
			switch (count) {
			case 0:
				word0[j] = '\0';
				break;
			case 1:
				word1[j] = '\0';
				break;
			case 2:
				word2[j] = '\0';
				break;
			}
			j = 0;
			count++;
			continue;
		}
		switch (count) {
		case 0:
			word0[j] = buff[i];
			break;
		case 1:
			word1[j] = buff[i];
			break;
		case 2:
			word2[j] = buff[i];
			break;
		}
		j++;
	}
	if (count == 2)
		word2[j] = '\0';
	else if (count == 0)
		word0[j] = '\0';
	if (*num == 1) {
		*num = -1;
		if (count) {
			for (j = 0; j < command_set; j++) {
				if (strcmp(word0, commands[j]) == 0) {
					*num = j;
				}
			}
		}
		else {
			for (j = command_set; j < command_len; j++) {
				if (strcmp(word0, commands[j]) == 0) {
					*num = j;
				}
			}
		}
	}
}

void analyse(char buff[DEFAULT_BUFLEN], char word1[DEFAULT_BUFLEN], char word2[DEFAULT_BUFLEN], int* num) {
	char specifier[DEFAULT_BUFLEN];
	*num = 1;
	analyse(buff, specifier, word1, word2, &*num);
}

int process_output(SOCKET localClientSocket, char name[15], int clientnumber, int num, char word1[DEFAULT_BUFLEN], char word2[DEFAULT_BUFLEN]) {
	int i, boolean = 0;
	int value;
	switch (num) {
	case 0://send one person
	case 1://send many person
		//word1 -> name/names
		//word2 -> message
		int number;
		char names[10][15];
		if (num == 1)
			split(names, word1, &number);
		else {
			number = 1;
			strcpy_s(names[0], word1);
		}
		char savemessage[DEFAULT_BUFLEN];
		char message[DEFAULT_BUFLEN];
		strcpy_s(savemessage, word2);
		for (int i = 0; i < number; i++) {
			cout << i + 1 << ")" << names[i] << endl;
			if (strcmp(name, names[i]) == 0) {
				cout << "same user" << endl;
				continue;
			}
			strcpy_s(message, savemessage);
			int value = client->sendoneperson(localClientSocket, names[i], message, name);
			if (!value)
			{
				cout << "Not online" << endl;
				value = myfiles->sendMessageUsingName(names[i], message, name, 0);
				if (value) {
					char newmessage[50];
					sprintf_s(newmessage, "message sent to %s\0", names[i]);
					sendd(localClientSocket, newmessage, name);
					cout << "Sent offline" << endl;
				}
				else {
					cout << "Not registered user" << endl;
				}
			}
			else {
				cout << "Online" << endl;
				value = myfiles->sendMessageUsingName(names[i], message, name, 1);
			}
		}
		break;
	case 2: //signup
	case 3: //login
		//word1 -> name
		//word2 -> password
		//char text[50];
		//sprintf_s(text, "%s_%s\0", word1, word2);
		if (num == 2) {
			boolean = loginid->Find(word1);	//boolean = myfiles->getname(word1); 
			if (boolean) boolean = 0; 
			else boolean = 1;
		}
		else if (num == 3)
			boolean = loginid->Find(word1, word2);	//boolean = myfiles->getid(text);
		
		if (boolean) {
			for (i = 0; i < strlen(word1); i++) {
				name[i] = word1[i];
			}
			name[i] = '\0';
			client->changename(localClientSocket, name);
			if (num == 2) {
				loginid->Append(word1, word2);	//myfiles->setid(text);
				cout << "New user created" << endl;
			}
			else if (num == 3) {
				myfiles->fetchmessageswhileoffline(name, localClientSocket);
				cout << "Sent fetched messages" << endl;
			}
		}
		else {
			char message[25];
			char tempname[15];
			sprintf_s(tempname, "Client-%d\0", clientnumber);
			if (num == 2)
				strcpy_s(message, "#User Already Exist");
			else if (num == 3)
				strcpy_s(message, "#Fail");
			return sendd(localClientSocket, message, tempname);
		}
		break;
	case 4: //logout
		name[0] = '\0';
		client->changename(localClientSocket, name, 0);
		break;
	case 5: //delete
	case 6: //remove
		loginid->Delete(name); //myfiles->deleteid(name);
		client->changename(localClientSocket, name, 2);
		name[0] = '\0';
		break;
	case 7: //myname
	case 8: //name
		client->myname(localClientSocket, name);
		break;
	case 9: //others
		client->getonline(localClientSocket, name);
		break;
	}
	return 1;
}



