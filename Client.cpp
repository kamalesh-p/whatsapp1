#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <thread>
#include <iostream>
using namespace std;

// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"

WSADATA wsaData;
SOCKET ConnectSocket = INVALID_SOCKET;
struct addrinfo* result = NULL,
    * ptr = NULL,
    hints;
char recvbuf[DEFAULT_BUFLEN];
int iResult;
int recvbuflen = DEFAULT_BUFLEN;
const char* sendbuf;

// Kamalesh
#define SLEEP 20
char statement[DEFAULT_BUFLEN] = "";

void connect_to_server();
void connect_to_server(int);
void sendd();
void receivee();

thread* t1, * t2;

bool counting = 0;

bool loggedin = false;

class threadsend {
public:
    void operator()(int num)
    {
        while (1) {
            sendd();
            Sleep(SLEEP);
        }
    }
};

class threadreceive {
public:
    void operator()(int num)
    {
        while (1) {
            receivee();
            Sleep(SLEEP);
        }
    }
};

void sendd() {
    if (!counting) {
        sprintf_s(statement, "%s\0", ""); // empty string
        cin.getline(statement, DEFAULT_BUFLEN);
        //scanf_s("%\ns", statement);
    }
    else {
        cout << statement << endl;
        counting = 0;
    }
    if (statement[0] == '\0') {
        return;
    }
    if (strcmp(statement, "clear") == 0 || strcmp(statement, "cls") == 0 || strcmp(statement, "clc") == 0) {
        system("cls");
        return;
    }
    else if (strcmp(statement, "login") == 0) {
        char name[15];
        char pass[15];
        cout << "Enter name: ";
        cin >> name;
        cout << "Enter password: ";
        cin >> pass;
        sprintf_s(statement, "login_%s_%s\0", name, pass);
    }
    else if (strcmp(statement, "signup") == 0) {
    labelsignup:
        char name[15];
        char pass[15];
        char conf[15];
        cout << "Enter name: ";
        cin >> name;
        cout << "Enter password: ";
        cin >> pass;
        cout << "Confirm password: ";
        cin >> conf;
        if (strcmp(pass, conf) != 0) {
            cout << "Password and Confirm Password does'nt match" << endl;
            goto labelsignup;
        }
        sprintf_s(statement, "signup_%s_%s\0", name, pass);
    }
    else if (loggedin) {
        if (strcmp(statement, "send group") == 0 || strcmp(statement, "send many") == 0) {
            cout << "Enter names: (for example)" << "user-1,user-2,user-3,user-4....." << endl;
            char names[15 * 10];
            char message1[DEFAULT_BUFLEN] = "\0";
            char message[DEFAULT_BUFLEN] = "\0";
            cin >> names;
            cout << "Enter message: ";
            cin >> message1;
            cin.getline(message, DEFAULT_BUFLEN);
            sprintf_s(statement, "names_%s_%s%s\0", names, message1, message);
        }
        else if (strcmp(statement, "send") == 0) {
            char name[15];
            char message1[DEFAULT_BUFLEN] = "\0";
            char message[DEFAULT_BUFLEN] = "\0";
            cout << "Enter receiver name: ";
            cin >> name;
            cout << "Enter message: ";
            cin >> message1;
            cin.getline(message, DEFAULT_BUFLEN);
            sprintf_s(statement, "name_%s_%s%s\0", name, message1, message);
        }
    }
    sendbuf = statement;
    iResult = send(ConnectSocket, sendbuf, strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        connect_to_server();
        //return 0;
    }
    cout << "sent: " << sendbuf << endl;
    //return 1;
}

void receivee() {
    // Receive data
    sprintf_s(recvbuf, "%s\0", ""); // empty string
    iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
    if (iResult > 0) {
        cout << "Received: ";
        int i;
        for (i = 0; i < iResult; i++) {
            cout << recvbuf[i];
        }
        recvbuf[i] = '\0';
        cout << endl;
        if (strcmp(recvbuf, "#Login Successful\0") == 0) {
            loggedin = true;
        }
        else if (strcmp(recvbuf, "#Fail\0") == 0) {
            loggedin = false;
            cout<<"Failed to login" << endl;
        }
        else if (strcmp(recvbuf, "#Logged out\0") == 0) {
            loggedin = false;
        }
        else if(strcmp(recvbuf, "#Account deleted\0") == 0) {
            loggedin = false;
        }
    }
    else if (iResult == 0) {
        printf("Connection closed\n");
        connect_to_server();
        //return 0;
    }
    else {//iResult = -1
        printf("Server disconnected\n");
        connect_to_server();
        //return 0;
    }
    //return 1;
}

char** argv2;
void connect_to_server() {
    connect_to_server(1);
}

void login() {
    return;
    //login
    char name[15], password[15];
    cout << "Enter name: ";
    //cin >> name;
    sprintf_s(name, "p1");
    cout << name << endl;

    cout << "Enter password: ";
    //cin >> password;
    sprintf_s(password, "p1");
    cout << password << endl;
    sprintf_s(statement, "login_%s_%s", name, password);
}

//login_name_passsword
void connect_to_server(int boolean) 
 {
    Sleep(SLEEP);
    if (boolean)
        system("cls");
    cout << "start connecting" << endl;
    // Resolve the server address and port
    iResult = getaddrinfo(argv2[1], DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
    }

    // Attempt to connect to an address until one succeeds
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {

        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
        }
        // Connect to server.
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        cout << ptr->ai_addr << "" << ptr->ai_socktype << endl;
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        connect_to_server();
    }
    cin.clear();
    //counting = 1;
    //login(); 
}

int __cdecl main(int argc, char** argv)
{
    argv2 = argv;
    // Validate the parameters
    if (argc != 2) {
        printf("usage: %s server-name\n", argv[0]);
        //return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 2;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC; //hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    connect_to_server();
    
    login();
    t1 = new thread(threadreceive(), 1);
    t2 = new thread(threadsend(), 1);
    t1->join();
    t2->join();

    return 0;
}