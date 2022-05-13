#undef UNICODE
#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <iostream> 

using namespace std;

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
// #pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

WSADATA wsaData;
int iResult;

SOCKET ListenSocket = INVALID_SOCKET;
SOCKET ClientSocket = INVALID_SOCKET;

struct addrinfo* result = NULL;
struct addrinfo hints;

int iSendResult;
char recvbuf[DEFAULT_BUFLEN];
int recvbuflen = DEFAULT_BUFLEN;

const char* sendbuf;

/**********
    
    kamalesh

**********/ 

thread* t, * t1, * t2;
char statement[DEFAULT_BUFLEN + 19] = "";

static int threadcount = 0;
int CountClientNumber = 0;

int receivee(SOCKET ClientSocketReceive, int ClientNumber, char ReceiveName[15]);
int sendd(SOCKET ClientSocketSend, char SendStatement[DEFAULT_BUFLEN], char SendName[15]);
void accept_client();
void client_left(SOCKET ClientSocketLeft);

void threadcountfunc() {
    //cout << "******" << std::thread::hardware_concurrency << "******" << endl;
}


// Functions 2
void split(char word[10][15], char sentence[DEFAULT_BUFLEN], int* num);
void analyse(char buff[DEFAULT_BUFLEN], char word0[DEFAULT_BUFLEN], char word1[DEFAULT_BUFLEN], char word2[DEFAULT_BUFLEN], int* num);
void analyse(char buff[DEFAULT_BUFLEN], char word1[DEFAULT_BUFLEN], char word2[DEFAULT_BUFLEN], int* num);
int process_output(SOCKET localClientSocket, char name[15], int clientnumber, int num, char word1[DEFAULT_BUFLEN], char word2[DEFAULT_BUFLEN]);

// Functions
#include "functions.h"

// Linked List
#include "LinkedList.h"
Clients* client;

// Myfiles
#include "myfiles.h"
Myfiles* myfiles;
loginId* loginid;

// Functions 2
#include "functions2.h"

class threadreceive {
public:
    SOCKET ClientSocketReceive;
    int ClientNumber;
    char name[15];
    threadreceive(SOCKET ClientSocketReceive, int ClientNumber, char name[15]) {
        this->ClientSocketReceive = ClientSocketReceive; 
        this->ClientNumber = ClientNumber;
        strcpy_s(this->name, name);
    }
    void operator()(int num) {
        cout << "Started receiving from " << name << endl;
        while (receivee(ClientSocketReceive, ClientNumber, name));
    }
};

char namestore[15];

class threadGetClient {
public:
    void operator()(int num) {
        while (1) {
            accept_client();
            statement[0] = '\0';
            t2 = new thread(threadreceive(ClientSocket, CountClientNumber, statement), 1);
            threadcount++;
            threadcountfunc();

            t1 = new thread(threadGetClient(), 1);
            threadcount++;
            threadcountfunc();

            t1->join();
            threadcount--;
            threadcountfunc();

            t2->join();
            threadcount--;
            threadcountfunc();
        }
    }
};

int receivee(SOCKET ClientSocketReceive, int CountClientNumber, char ReceiveName[15]) {
    iResult = recv(ClientSocketReceive, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        
        recvbuf[iResult] = '\0';
        char word1[DEFAULT_BUFLEN], word2[DEFAULT_BUFLEN];
        int num;

        // analyse users code
        analyse(recvbuf, word1, word2, &num);
        cout << "from " << ReceiveName << ": " << recvbuf << endl;
        // check if signed in user
        if (num != 2 && num != 3 && !strlen(ReceiveName) && ReceiveName[0] == '\0') {
            char message[] = "#Login";
            char tempname[15];
            sprintf_s(tempname, "Client-%d\0", CountClientNumber);
            return sendd(ClientSocketReceive, message, tempname);
        }
        if (num == -1) {
            return 1;
        }
        else {
            // output
            return process_output(ClientSocketReceive, ReceiveName, CountClientNumber, num, word1, word2);
        }
    }
    else if (iResult == 0) {
        printf("Connection closing...\n");
        client_left(ClientSocketReceive);
    }
    else {
        client_left(ClientSocketReceive);
        // left online
        return 0;
    }
    return 1;
}


void client_left(SOCKET ClientSocketLeft) {
    client->remove(ClientSocketLeft);
    cout << "client left the clan" << endl;
}

int sendd(SOCKET ClientSocketSend, char SendStatement[DEFAULT_BUFLEN], char SendName[15]) {
    // Send
    const char* sendbuf = SendStatement;
    iSendResult = send(ClientSocketSend, sendbuf, strlen(sendbuf), 0);
    if (iSendResult == SOCKET_ERROR) {
        printf("ERROR: send failed\n");
        client_left(ClientSocketSend);
        return 0;
        // left online
    }
    cout << "sent to " << SendName << ": "<<sendbuf << endl;
    return 1;
}

void accept_client() {
    // Accept a client socket
    cout << "Server Listening for new Client" << endl;
    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == INVALID_SOCKET) {
        printf("accept failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
    }
    cout << "Client Accepted" << endl;
    CountClientNumber++;
    sprintf_s(statement, "%s\0", namestore);
    client->append(ClientSocket, statement);
}


int __cdecl main(void)
{
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    printf("Server start\n");
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET; // hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);
    //SOMAXCONN
    iResult = listen(ListenSocket, 1);
    //cout << "iresult" << iResult << endl;
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

/**********

    kamalesh

**********/
    myfiles = new Myfiles();
    loginid = new loginId();
    client = new Clients();
    t = new thread(threadGetClient(), 1);
    //threadcount++;
    //threadcountfunc();
    t->join();
    threadcount--;
    threadcountfunc();
    delete(t);
    return 100;
}