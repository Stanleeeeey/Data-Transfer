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
#define BUFF 10
#define BUFFID 3
#define ENDING "<!--END"
string TYPES[5] =  {"MOU", "SCR", "MIC", "KEY", "SOU"};


struct Message {
    int8_t type;
    int8_t id;
    char content[BUFF-BUFFID];

};

struct Send_args {
    string Batched[1000];
    
};

SOCKET SendSock = INVALID_SOCKET;
SOCKET RecvSock = INVALID_SOCKET;

struct sockaddr_in SendSockAddr;
struct sockaddr_in RecvSockAddr;

char SendMsgBuffered[BUFF + BUFFID + 1];
char RecvBufferedMsg[BUFFID + 1]; 

string Batched[1000];

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
int BuildSocket(){


    SendSockAddr.sin_family = AF_INET;
    SendSockAddr.sin_port   = htons(PORT);

    RecvSockAddr.sin_family = AF_INET;
    RecvSockAddr.sin_port   = htons(PORT+1);

    InetPton(AF_INET, _T(HOST), &RecvSockAddr.sin_addr.s_addr);
    InetPton(AF_INET, _T(HOST), &SendSockAddr.sin_addr.s_addr);

    if ( bind(RecvSock, (SOCKADDR *) & RecvSockAddr, sizeof (RecvSockAddr))) {
        printf("//  bind failed with error %d\n", WSAGetLastError());
        return 1;
    }

    printf("//  Socket succesfully build\n");
    return 0;

}

//RECIVING 
int ReciveFunc() {
    
    
    int ServerAddrSize  = sizeof(RecvSockAddr);


    int recived = recvfrom(RecvSock, RecvBufferedMsg, BUFFID, 0 , (SOCKADDR *)& RecvSockAddr, &ServerAddrSize);

    if (recived == SOCKET_ERROR) {
        printf("// recvfrom failed with error %d\n", WSAGetLastError());
        return 1;
    }


    RecvBufferedMsg[recived] = '\0';
    int x; 

    if (RecvBufferedMsg == "000"){
        x= 0;
    }else{
    std::stringstream str(RecvBufferedMsg); 
    str >> x;
    }

    //printf("//  succesfully recived :\n////  %s\n", RecvBufferedMsg);
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
        //printf("// Sending back response got an error: %d\n", WSAGetLastError());
        return 1;
    }

    //cout << SendMsgBuffered << endl;
    return 0;
}

//CONVERT TO BATCHES 
int IntoBatches(string message, int type){


    string Ending = ENDING;
    message       = message + Ending;
    string part;


    int TypesCount = TYPES[0].length();
    int IdCount    = BUFFID;
    int LocalBUFF  = BUFF;
    int length     = ceil((message.size())*1.0/(LocalBUFF-4));

    
    if (LocalBUFF < Ending.size()) {
        LocalBUFF = Ending.size();
    }

    string ans[1000];

    while (length > 999){
        cout<<"Increasing Buff Size ACTUAL BUFF SIZE:"<<LocalBUFF+1<<endl;

        LocalBUFF = LocalBUFF+1;
        length    = ceil((message.size())*1.0 /(LocalBUFF-4));
    }

    for (int i = 0; i < length; i++){
        
        stringstream ss;

        ss << setw(3) << setfill('0') << (i)%1000;
        string s = ss.str();
        

        
        if (message.size()>=(LocalBUFF-4)){
            
            part    = TYPES[type][0] + s + message.substr(0,LocalBUFF-4);
            message = message.substr(LocalBUFF-4, message.size());

            ans[i]  = part;

        }else{
            
            part = "";

            part  = TYPES[type][0] + s + message;

            
            ans[i] = "";
            ans[i] = part;

            //cout << ans[i] << endl;

        }
        

    }

    for (int i = 0; i<length; i++){
        Batched[i] = ans[i];
    }
    

    
    return 0;
}

//BATCH AND SENDFUNC
void Send(Send_args args){

    printf("////    Send Thread succesfully started\n");
    
    
    for (int i = 0; i<1000; i++){
        int lost = rand() % 100 + 1;
        
        if (args.Batched[i] == ""){
            break;
        }

        if (lost > 5) {
            SendFunc(args.Batched[i]);
        }
        
    }

    cout<<"////     Ended SendingThread"<<endl;
    
    
    
}

void Recive(Send_args args){
    
    
    string Batched[1000];
    int NumElem;
    int Failed;

    
    for (int i = 0; i<1000; i++){
        if (args.Batched[i] == ""){
            NumElem = i;
            break;
        }
        Batched[i] = args.Batched[i];
    }

   
    bool LastID = false;
    int ID;

    printf("////    Recive Thread Succesfully Started\n");



    while (1 == 1) {
        Failed = 0;

        ID = ReciveFunc();
        

        Batched[ID] = "0";

        if (ID + 1 == NumElem) {
            LastID = true;
        }

        for (int i = 0; i < ID; i++) {
            if (Batched[i]!="0") {
                cout << i << endl;
                Failed++;
                
                SendFunc(Batched[i]);
            }
        }
        if (Failed == 0 and LastID) {
            cout << "END"<<endl;
            break;
        }
    }

    printf("//Recive Thread Succesfully ended");
}

int Initialize(){
    
    int ID = -1;


    if (WIN){    
        InitializeWinSock();
    }

    InitializeSocket();
    BuildSocket();


    return 0;

}