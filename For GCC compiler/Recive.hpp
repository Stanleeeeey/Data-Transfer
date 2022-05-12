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
#include <pthread.h>
#include <tchar.h>

using namespace std;

// Zmienne globalne
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
int InitializeWinSock(){


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
int InitializeSocket(){


    SendSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    RecvSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

    if (SendSock != INVALID_SOCKET && RecvSock != INVALID_SOCKET){
        printf("//  Sockets succesfully initialized\n");
        return 0;
        
    }

    printf("//  Sockets initialization failed %d\n", WSAGetLastError());
    return 1;

}

//BUILD SOCKET
int BuildSocket(string host, int port) {

    char* char_arr;
    char_arr = &host[0];


    SendSockAddr.sin_family = AF_INET;
    SendSockAddr.sin_port = htons(port+1);

    RecvSockAddr.sin_family = AF_INET;
    RecvSockAddr.sin_port = htons(port);
    
    SendSockAddr.sin_addr.s_addr = inet_addr(char_arr);
    RecvSockAddr.sin_addr.s_addr  = inet_addr(char_arr);

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
    
    int ServerAddrSize  = sizeof(RecvSockAddr);


    int recived = recvfrom(RecvSock, RecvBufferedMsg, 1024, 0 , (SOCKADDR *)& RecvSockAddr, &ServerAddrSize);

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
int SendFunc(std::string x){

    for (int i =0; i<x.length(); i++) {
        SendMsgBuffered[i] = x[i];
        }

    int MsgLen = (int)(x.length());
    int result = sendto(SendSock, SendMsgBuffered, MsgLen, 0 , (SOCKADDR *) & SendSockAddr, sizeof(SendSockAddr));

    if (result == SOCKET_ERROR) {
        printf("// Got an error when sending message back: %d\n", WSAGetLastError());
        return 1;
    }


    return 0;
}

Message ReciveWithoutInit(){

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
        cout<<msg<<endl;

        
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

            cout << "RECIVED LAST MESSAGE" << endl;
            LastID = ID;

            
        }

        
        if (LastID != -1) {
            for (int i = 0; i <= LastID; i++) {
                
                if (FinMessage.value[i] != "")
                    Recived++;
            }
        }

        if (Recived == LastID+1) {
            cout << "END" << endl;
            break;
        }


    }
    
    return FinMessage;
    
}

int Initialize(string host, int port) {

    int ID = -1;


    if (WIN) {
        InitializeWinSock();
    }

    InitializeSocket();
    if(!BuildSocket(host, port)){
        return 1;
    }


    return 0;

}

string Recive(string host, int port) {
    if(!Initialize(host, port)){
        return "Error occured while binding";
    }
    string ans;
    Message X =  ReciveWithoutInit();
    for (int i = 0; i < 1000; i++) {
        if (X.value[i] != "0") {
            ans += X.value[i];
        }
    }

    
    
    return ans;
    }
