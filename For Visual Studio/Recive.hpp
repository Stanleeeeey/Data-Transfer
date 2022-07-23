
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
#include <tuple>
#include <sstream>
#include <iomanip>
#include "thread"
#include <tchar.h>
#include <chrono>
#include <atomic>



using namespace std;
using namespace std::chrono;

// Zmienne globalne
#define BUFF 1024
#define BUFFID 3

char SendMsgBuffered[BUFFID];
char RecvBufferedMsg[BUFF+BUFFID];

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
        SendSockAddr.sin_family = AF_INET;
        SendSockAddr.sin_port = htons(port + 1);

        RecvSockAddr.sin_family = AF_INET;
        RecvSockAddr.sin_port = htons(port);

    }

    void convert_to_ip6(int port, string host) {

        wstring stemp = std::wstring(host.begin(), host.end());
        LPCWSTR sw = stemp.c_str();

        InetPton(AF_INET, sw, &RecvSockAddr.sin_addr.s_addr);
        InetPton(AF_INET, sw, &SendSockAddr.sin_addr.s_addr);
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
        convert_to_ip6(port, host);
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

class reciveconnection {
private:

    ReciveSockets ClientSockets;

    

    // Functions to recive data from client 

    void is_message_valid(int recived) {
        //
        //  checks if message valid
        //
        if (recived == SOCKET_ERROR) {
            printf("// recvfrom failed with error %d\n", WSAGetLastError());
            abort();
        }
    }

    string single_packet_recive() {
        //
        //  Recives single packet 
        //

        char RecvBufferedMsg[BUFF];

        int ServerAddrSize = sizeof(ClientSockets.RecvSockAddr);


        int recived = recvfrom(ClientSockets.RecvSock, RecvBufferedMsg, 1024, 0, (SOCKADDR*)&ClientSockets.RecvSockAddr, &ServerAddrSize);

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

    


    //Functions to Send

    bool is_socket_error(int result) {
        //
        //  checks if there is a socket error
        //

        if (result == SOCKET_ERROR) {
            printf("// while sending got an error: %d\n", WSAGetLastError());
            return 1;
        }

        return 0;
    }

    int send_single_packet(std::string x) {
        //
        //  Sends single string to the sender
        //
        // x: string to send


        for (int i = 0; i < x.length(); i++) {
            SendMsgBuffered[i] = x[i];
        }

        int result = sendto(ClientSockets.SendSock, SendMsgBuffered, x.length(), 0, (SOCKADDR*)&ClientSockets.SendSockAddr, sizeof(ClientSockets.SendSockAddr));

        return is_socket_error(result);
    }

 

    string recive_and_resend() {
        /// 
        /// recives data resend if needed
        /// 
        
        
        int Recived = 0;
        string msg;

        int FinMessage[1000] = {0};


        int ID;
        int LastID = -2;
        int size = 0;

        string ans[1000] = { "" };

        while (1 == 1) {
            Recived = 0;
            msg = single_packet_recive();


            if (msg.substr(1, 3) == "000") {
                ID = 0;
                ans[ID] =  msg.substr(3, msg.size());
            }
            else {
                std::stringstream str(msg.substr(0, 3));
                str >> ID;
                ans[ID] = msg.substr(3, msg.size());

            }


            send_single_packet(to_string(ID));
            
            FinMessage[ID] = msg.size();
            if (FinMessage[0] > msg.size()) {
                LastID = ID;
                cout << "LAST ID RECIVED\n";
            }


            if (count(FinMessage, FinMessage + 1000, true) == LastID + 1) {
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
    reciveconnection(string userhost, int userport) {

        ClientSockets.initalize(userport, userhost); //initialize socket

    }



    string Recive() {
        //
        //  Recives data from Sender
        //
        
        string ans = recive_and_resend();

        ClientSockets.close_socket();

        return ans;
    }

};







string Recive(string host, int port) {
    reciveconnection x = reciveconnection(host, port);
    return x.Recive();
    }
