#pragma once
//Linked List
class Node {
public:
    char name[15];
    SOCKET ClientSocket;
    Node* next;
    long int id;
    Node() {
        name[0] = '\0';
    }
};

class Clients {
public:
    Node* head = NULL;
    int length;
    int wait;
    char name[DEFAULT_BUFLEN];
    Clients() {
        length = 0; 
        wait = 0;
    }
    void myname(SOCKET ClientSocketmyname, char name[15]) {
        Node* thisNode = head;
        while (thisNode != NULL) {
            if (ClientSocketmyname == thisNode->ClientSocket) {
                char message[50] = "\0";
                sprintf_s(message, "func(%s), list(%s)\0", name, thisNode->name);
                if (!sendd(ClientSocketmyname, message, thisNode->name))return;
                break;
            }
            thisNode = thisNode->next;
        }
    }
    void remove(SOCKET ClientSocket) {
        while (this->wait) {
            cout << "Remove waiting...." << endl;
            Sleep(50);
        }
        this->wait = 1;
        Node* prev = head; // empty header
        Node* current = head->next; // the first valid node
        while (current != NULL && current->next != NULL) {
            if (current->ClientSocket == ClientSocket) {
                break;
            }
            else {
                prev = current;
                current = current->next; // go to next value
            }
        }
        if (head != NULL && head->next == NULL && head->ClientSocket == ClientSocket) {
            cout << "Deleting(" << head->name << ")" << endl;
            head = NULL;
        }
        else if (current == NULL) { // if we reached end of list or the list is empty
            cout << "Can't remove value: no match found.\n";
            //if (count < 5) {
                //Sleep(100);
                //this->wait = 0;
                //remove(ClientSocket, count++);
            //}
        }
        else {
            cout << "Deleting(";
            prev->next = current->next; // unlink the node you remove
            cout << current->name << ")" << endl;
            delete current; // delete the node
        }
        Sleep(100);
        this->wait = 0;
    }
    void append(SOCKET ClientSocket, char name[15]) {
        while (this->wait) {
            cout << "Append waiting...." << endl;
            Sleep(50);
        }
        this->wait = 1;
        Node* currentNode = new Node();
        currentNode->ClientSocket = ClientSocket;
        strcpy_s(currentNode->name, name);
        currentNode->next = NULL;
        if (head == NULL) {
            cout << "Append successful\n";
            head = currentNode;
        }
        else {
            Node* thisNode = head;
            while (thisNode != NULL && thisNode->next != NULL) {
                //cout << thisNode->name << endl;
                thisNode = thisNode->next;
            }
            //cout << thisNode->name << endl;
            cout << "Append successful\n";
            thisNode->next = currentNode;
        }
        length++;
        Sleep(100);
        this->wait = 0;
    }
    int changename(SOCKET ClientSocketchangename, char chngenome[15], int num = 1) {
        Node* thisNode = head;
        while (thisNode != NULL) {
            if (ClientSocketchangename == thisNode->ClientSocket) {
                strcpy_s(thisNode->name, chngenome);
                char message[20];
                if (num == 1) {
                    strcpy_s(message, "#Login Successful");
                }
                else if (num == 2) {
                    strcpy_s(message, "#Account deleted");
                }
                else {
                    strcpy_s(message, "#Logged out");
                }
                if (!sendd(ClientSocketchangename, message, thisNode->name))return 0;
                break;
            }
            thisNode = thisNode->next;
        }
        return num;
    }
    void getonline(SOCKET ClientSocketgetonline, char SendName[15]) {
        Node* thisNode = head;
        name[0] = '\0';

        while (thisNode != NULL) {
            if (ClientSocketgetonline != thisNode->ClientSocket && thisNode->name[0] != '\0') {
                //cout << thisNode->name << endl;
                if (strlen(name) + strlen(thisNode->name) >= DEFAULT_BUFLEN) {
                    if (!sendd(ClientSocketgetonline, name, SendName))return;
                    sprintf_s(name, "\0");//empty string
                }

                int j = strlen(name);
                int i;
                for (i = 0; i < strlen(thisNode->name); i++) {
                    name[j + i] = thisNode->name[i];
                }
                name[j + i] = '\n';
                name[j + i + 1] = '\0';
            }
            thisNode = thisNode->next;

        }
        if (!sendd(ClientSocketgetonline, name, SendName))return;
    }
    int sendonepersonfromserver(char servermessage[DEFAULT_BUFLEN], char fromname[15], char toname[15]) {
        Node* thisNode = head;
        while (thisNode != NULL) {
            if (strcmp(thisNode->name, fromname) == 0) {
                char newmessage[DEFAULT_BUFLEN] = "\0";
                sprintf_s(newmessage, "%s(seen by %s)(server)", servermessage, toname);
                cout << newmessage << endl;
                if (!sendd(thisNode->ClientSocket, newmessage, fromname))return 0;
                return 1;
            }
            thisNode = thisNode->next;
        }
        return 0;
    }
    int sendoneperson(SOCKET ClientSocketsendoneperson, char ReceiveName[15], char sendmessage[DEFAULT_BUFLEN], char SendName[15]) {
        Node* thisNode = head;
        char message[DEFAULT_BUFLEN];
        while (thisNode != NULL) {
            if (strcmp(thisNode->name, ReceiveName) == 0 && ClientSocketsendoneperson != thisNode->ClientSocket) {
                int i;
                sprintf_s(message, "%s\0", sendmessage);
                int k = strlen(message);
                message[k] = '('; k++;
                for (i = 0; i < strlen(SendName); i++) {
                    message[k + i] = SendName[i];
                }
                message[k + i] = ')';
                message[k + i + 1] = '\0';
                if (!sendd(thisNode->ClientSocket, message, ReceiveName))return 0;
                char newmessage[17 + 15] = "message seen by \0";
                k = strlen(newmessage);

                for (i = 0; i < strlen(ReceiveName); i++) {
                    newmessage[k + i] = ReceiveName[i];
                }
                newmessage[k + i] = '\0';
                if (!sendd(ClientSocketsendoneperson, newmessage, SendName))return 0;
                return 1;
            }
            thisNode = thisNode->next;
        }
        //sendd(ClientSocketsendoneperson, newmessage, SendName)
        return 0;
    }
};


