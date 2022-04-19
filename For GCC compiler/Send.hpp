#ifdef _WIN32

#   include <winsock2.h>
#   include <Ws2tcpip.h>
#   include <windows.h>
#   pragma comment(lib, "Ws2_32.lib")

#   define WIN TRUE


#else

#include <sys/socket.h>

#endif

#include <stdio.h>
#include <iostream>
#include <cmath>
#include <typeinfo>
#include <vector>
#include <list>
#include <sstream>
#include <iomanip>
#include <tchar.h>
#include <stdlib.h>
#include <pthread.h>
#include <chrono>

using namespace std;


// Zmienne globalne
#define PORT 6666
#define HOST "127.0.0.1"
#define BUFF 10
#define BUFFID 3



struct Send_args {
    string Batched[1000];

};

struct Timer_args {
    Send_args message;
    int Time;

};


SOCKET SendSock = INVALID_SOCKET;
SOCKET RecvSock = INVALID_SOCKET;

struct sockaddr_in SendSockAddr;
struct sockaddr_in RecvSockAddr;

char SendMsgBuffered[BUFF + BUFFID + 1];
char RecvBufferedMsg[BUFFID + 1];


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

int CloseSocket() {

    closesocket(SendSock);
    closesocket(RecvSock);
    WSACleanup();
    return 1;
}
//BUILD SOCKET
int BuildSocket(string host, int port) {

    
    const char *sw = host.c_str();

    SendSockAddr.sin_family = AF_INET;
    SendSockAddr.sin_addr.S_un.S_addr = inet_addr(sw);
    SendSockAddr.sin_port = htons(port);

    RecvSockAddr.sin_family = AF_INET;
    RecvSockAddr.sin_addr.S_un.S_addr = inet_addr(sw);
    RecvSockAddr.sin_port = htons(port + 1);


    if (bind(RecvSock, (SOCKADDR*)&RecvSockAddr, sizeof(RecvSockAddr))) {
        printf("//  bind failed with error %d\n", WSAGetLastError());
        return 1;
    }

    printf("//  Socket succesfully build\n");
    return 0;

}



//RECIVING returns data packet id
int ReciveFunc() {


    int ServerAddrSize = sizeof(RecvSockAddr);



    int length = sizeof(ServerAddrSize);


    int recived = recvfrom(RecvSock, RecvBufferedMsg, BUFFID, 0, (SOCKADDR*)&RecvSockAddr, &ServerAddrSize);

    if (recived == SOCKET_ERROR) {
        printf("// recvfrom failed with error %d\n", WSAGetLastError());
        return 1;
    }


    RecvBufferedMsg[recived] = '\0';
    int x;

    if (RecvBufferedMsg == "000") {
        x = 0;
    }
    else {
        std::stringstream str(RecvBufferedMsg);
        str >> x;
    }

    return x;

}

//SEND given string 
int SendFunc(std::string x) {
    cout<<"sending "<<x<<endl;


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


//CONVERT TO BATCHES 
Send_args IntoBatches(string message) {


    message = message;
    string part;

    int IdCount = BUFFID;
    int LocalBUFF = BUFF;
    int length = ceil((message.size()) * 1.0 / (LocalBUFF - 4));



    Send_args ans;

    while (length > 999 || length % LocalBUFF == 0) {
        cout << "Increasing Buff Size ACTUAL BUFF SIZE:" << LocalBUFF + 1 << endl;

        LocalBUFF = LocalBUFF + 1;
        length = ceil((message.size()) * 1.0 / (LocalBUFF - 4));
    }

    for (int i = 0; i < length; i++) {

        stringstream ss;

        ss << setw(3) << setfill('0') << (i) % 1000;
        string s = ss.str();



        if (message.size() >= (LocalBUFF - 4)) {

            ans.Batched[i] = s + message.substr(0, LocalBUFF - 4);
            message = message.substr(LocalBUFF - 4, message.size());


        }
        else {

            ans.Batched[i] = s + message;

        }


    }


    return ans;
}

//SEND batched data
void SendWithoutChecking(Send_args args) {

    printf("////    Send Thread succesfully started\n");


    for (int i = 0; i < 1000; i++) {

        cout<<args.Batched[i]<<endl;
        if (args.Batched[i] == "") {
            break;
        }


        SendFunc(args.Batched[i]);

    }

    cout << "////     Ended Sending Thread" << endl;



}

//Recive Recives messages and makes sure taht every message is delivered
void *ReciveSendingSide(void *input) {

    struct Send_args* args = ((struct Send_args*)input);

    int NumElem;
    int Failed;


    for (int i = 0; i < 1000; i++) {
        if (args->Batched[i] == "") {
            NumElem = i;
            break;
        }
    }


    bool LastID = false;
    int ID = -1;

    printf("////    Recive Thread Succesfully Started\n");

    Send_args Recived;

    while (1 == 1) {

        Failed = 0;
        ID = ReciveFunc();

        

        Recived.Batched[ID] = "0";

        if (ID + 1 == NumElem) {
            LastID = true;
        }

        for (int i = 0; i < ID + 1; i++) {
            if (Recived.Batched[i] != "0") {

                Failed++;

                SendFunc(args->Batched[i]);
            }
        }

        if (Failed == 0 & LastID) {
            
            cout << "END" << endl;
            break;
            //return 0;
        }

        

    }


    //return 1;
}



int Initialize(string host, int port) {

    int ID = -1;


    if (WIN) {
        InitializeWinSock();
    }

    InitializeSocket();
    BuildSocket(host, port);


    return 0;

}




int Send(string msg, string host, int port, int timeout) {

    cout << "//Starting Winsock Initialize and Socket Build" << endl;
    Initialize(host, port);


    Send_args Batched = IntoBatches(msg);

    struct Send_args args;
    struct Send_args *Allen = (struct Send_args *)malloc(sizeof(struct Send_args));

    for (int i = 0; i < 1000; i++) {

        if (Batched.Batched[i] == "") {
            break;
        }
        args.Batched[i] = Batched.Batched[i];
        Allen->Batched[i] = Batched.Batched[i];
    }


    Timer_args data;
    data.message = args;
    data.Time = 1000;

    pthread_t SendThread, ReciveThread;

    SendWithoutChecking(args);
    cout<<"sended"<<endl;

    pthread_create(&ReciveThread, NULL, ReciveSendingSide, (void *)Allen);
    pthread_join(ReciveThread, NULL);

    



    CloseSocket();



    return 0;

}