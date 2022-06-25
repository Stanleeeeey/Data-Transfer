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
#include <tchar.h>

using namespace std;

// Zmienne globalne
#define BUFF 1024
#define BUFFID 3

class ReciveSockets {
private:


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


    //Initializng sockets
    int check_if_socket_valid() {
        if (SendSock != INVALID_SOCKET && RecvSock != INVALID_SOCKET) {
            printf("//  Sockets succesfully initialized\n");
            return 0;

        }

        printf("//  Sockets initialization failed %d\n", WSAGetLastError());
        return 1;
    }
    int InitializeSocket() {


        SendSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        RecvSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

        return check_if_socket_valid();

    }

    void adress_sockets(int port, string host) {

        char* char_arr;
        char_arr = &host[0];
        SendSockAddr.sin_family = AF_INET;
        SendSockAddr.sin_port = htons(port + 1);

        RecvSockAddr.sin_family = AF_INET;
        RecvSockAddr.sin_port = htons(port);

        SendSockAddr.sin_addr.s_addr = inet_addr(char_arr);
        RecvSockAddr.sin_addr.s_addr  = inet_addr(char_arr);

    }


    int attempt_biniding_recive_sock() {
        if (bind(RecvSock, (SOCKADDR*)&RecvSockAddr, sizeof(RecvSockAddr))) {
            printf("//  bind failed with error %d\n", WSAGetLastError());
            return 1;
        }

        printf("//  Socket succesfully binded\n");
        return 0;
    }
    //build sockets
    int BuildSocket(int port, string host) {

        adress_sockets(port,  host);
        return attempt_biniding_recive_sock();

    }

public:
    SOCKET SendSock = INVALID_SOCKET;
    SOCKET RecvSock = INVALID_SOCKET;

    struct sockaddr_in SendSockAddr;
    struct sockaddr_in RecvSockAddr;

    int close_socket() {

        closesocket(SendSock);
        closesocket(RecvSock);
        WSACleanup();
        return 1;
    }

    int initalize(int port, string host) {



        if (WIN) {
            InitializeWinSock();
        }


        if (InitializeSocket() || BuildSocket(port, host)) {
            printf("aborting process due to above error");
            return 1;
        }


        return 0;
    }

};


//COMPILE COMMAND 
//g++ -o prog.exe Recive.cpp -lws2_32 -pthread



class Reciveconnnection{
private:
    ReciveSockets s; 

    char SendMsgBuffered[BUFF - 4];
    //RECIVING 
    string ReciveFunc() {
        char RecvBufferedMsg[BUFF];
    
        int ServerAddrSize  = sizeof(s.RecvSockAddr);


        int recived = recvfrom(s.RecvSock, RecvBufferedMsg, 1024, 0 , (SOCKADDR *)& s.RecvSockAddr, &ServerAddrSize);

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
        int result = sendto(s.SendSock, SendMsgBuffered, MsgLen, 0 , (SOCKADDR *) & s.SendSockAddr, sizeof(s.SendSockAddr));

        if (result == SOCKET_ERROR) {
            printf("// Got an error when sending message back: %d\n", WSAGetLastError());
            return 1;
        }


        return 0;
    }

    string ReciveWithoutInit(){

        int Recived = 0;
        string msg;

        int FinMessage[1000] = {0};


        int ID;
        int LastID = -2;
        int size = 0;

        string ans[1000] = { "" };

        while (1 == 1) {
            Recived = 0;
            msg = ReciveFunc();


            if (msg.substr(1, 3) == "000") {
                ID = 0;
                ans[ID] =  msg.substr(3, msg.size());
            }
            else {
                std::stringstream str(msg.substr(0, 3));
                str >> ID;
                ans[ID] = msg.substr(3, msg.size());

            }


            SendFunc(to_string(ID));
            
            FinMessage[ID] = msg.size();
            if (FinMessage[0] > msg.size()) {
                LastID = ID;
                cout << "LAST ID RECIVED\n";
            }

            if (LastID != -1) {
                for (int i = 0; i <= LastID; i++) {

                    if (FinMessage[i])
                        Recived++;
                }
            }
            
            if (Recived == LastID + 1) {
                cout << "END" << endl;
                break;
            }


        }
        string finalans = "";
        for (int i = 0; i < LastID + 1; i++) {
            finalans += ans[i];
        }
        return finalans;

    }




    public:

    Reciveconnnection(string host, int port){
        s.initalize(port, host);
    }

    string Recive() {
        return ReciveWithoutInit();
    }
};

string Recive(string host, int port){
    Reciveconnnection connection = Reciveconnnection(host,port);
    return connection.Recive();
}