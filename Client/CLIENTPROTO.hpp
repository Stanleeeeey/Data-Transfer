#ifdef _WIN32

#   include <winsock2.h>
#   include <Ws2tcpip.h>
#   include <windows.h>
#   pragma comment(lib, "Ws2_32.lib")

#   define WIN TRUE


#else

#include <sys/sockets.h>

#endif

#include <stdio.h>
#include <iostream>
#include <cmath>
#include <typeinfo>
#include <vector>
#include <list>
#include <sstream>
#include <iomanip>
#include <thread>
#include <tchar.h>

using namespace std;

// Zmienne globalne
#define PORT 6666
#define HOST "127.0.0.1"
#define BUFF 1024
#define BUFFID 3




struct Message {
    string value[1000];
};

SOCKET SendSock = INVALID_SOCKET;
SOCKET RecvSock = INVALID_SOCKET;

struct sockaddr_in SendSockAddr;
struct sockaddr_in RecvSockAddr;


char SendMsgBuffered[BUFF - 4];


//COMPILE COMMAND 
//g++ -o prog.exe RPserver.cpp -lws2_32 -pthread

//INITIALIZE WINSOCK LIBRARY
int InitializeWinSock() {


    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (res != 0) {
        printf("//  WSAStartup failed with error %d\n", res);
        return 1;
    }
    printf("//  Winsock Initialized\n");
    return 0;

}

//INITIALIZE SOCKET 
int InitializeSocket() {


    SendSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    RecvSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (SendSock != INVALID_SOCKET && RecvSock != INVALID_SOCKET) {
        printf("//  Sockets succesfully initialized\n");
        return 0;

    }

    printf("//  Sockets initialization failed %d\n", WSAGetLastError());
    return 1;

}

//BUILD SOCKET
int BuildSocket() {


    SendSockAddr.sin_family = AF_INET;
    SendSockAddr.sin_port = htons(PORT + 1);

    RecvSockAddr.sin_family = AF_INET;
    RecvSockAddr.sin_port = htons(PORT);


    InetPton(AF_INET, _T(HOST), &RecvSockAddr.sin_addr.s_addr);
    InetPton(AF_INET, _T(HOST), &SendSockAddr.sin_addr.s_addr);

    if (bind(RecvSock, (SOCKADDR*)&RecvSockAddr, sizeof(RecvSockAddr))) {
        printf("//  bind failed with error %d\n", WSAGetLastError());
        return 1;
    }

    printf("//  Socket succesfully build\n");
    return 0;

}

//RECIVING 
string ReciveFunc() {
    char RecvBufferedMsg[BUFF];

    int ServerAddrSize = sizeof(RecvSockAddr);


    int recived = recvfrom(RecvSock, RecvBufferedMsg, 1024, 0, (SOCKADDR*)&RecvSockAddr, &ServerAddrSize);

    if (recived == SOCKET_ERROR) {
        printf("// recvfrom failed with error %d\n", WSAGetLastError());
        return "1";
    }


    RecvBufferedMsg[recived] = '\0';

    string x;

    for (int i = 0; i < recived; i++) {

        x.push_back(RecvBufferedMsg[i]);
    }


    return x;

}

//SEND
int SendFunc(std::string x) {

    for (int i = 0; i < x.length(); i++) {
        SendMsgBuffered[i] = x[i];
    }

    int MsgLen = (int)(x.length());
    int result = sendto(SendSock, SendMsgBuffered, MsgLen, 0, (SOCKADDR*)&SendSockAddr, sizeof(SendSockAddr));

    if (result == SOCKET_ERROR) {
        printf("// Sending back response got an error: %d\n", WSAGetLastError());
        return 1;
    }


    return 0;
}

Message ReciveWithoutInit() {

    int Recived = 0;


    string msg;

    Message FinMessage;
    for (int i = 0; i < 1000; i++) {
        FinMessage.value[i] = "";
    }

    int ID;
    int LastID = -2;

    while (1 == 1) {
        Recived = 0;
        msg = ReciveFunc();


        if (msg.substr(1, 3) == "000") {
            ID = 0;
        }
        else {
            std::stringstream str(msg.substr(0, 3));
            str >> ID;

        }


        SendFunc(msg.substr(0, 3));

        FinMessage.value[ID] = msg.substr(3, msg.size());

        if (msg.size() < FinMessage.value[0].size()) {

            cout << "TO JEST JUZ KONIEC" << endl;
            LastID = ID;


        }


        if (LastID != -1) {
            for (int i = 0; i <= LastID; i++) {

                if (FinMessage.value[i] != "")
                    Recived++;
            }
        }

        if (Recived == LastID + 1) {
            cout << "END" << endl;
            break;
        }


    }

    return FinMessage;

}

int Initialize() {
    int PreaviousID;
    int ID = -1;


    if (WIN) {
        InitializeWinSock();
    }

    InitializeSocket();
    BuildSocket();


    return 0;

}

string Recive() {
    Initialize();
    string ans;
    Message X = ReciveWithoutInit();
    for (int i = 0; i < 1000; i++) {
        if (X.value[i] != "0") {
            ans += X.value[i];
        }
    }



    return ans;
}
