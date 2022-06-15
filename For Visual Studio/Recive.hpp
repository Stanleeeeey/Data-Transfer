/*#ifdef _WIN32

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
int BuildSocket(string host, int port) {

    wstring stemp = std::wstring(host.begin(), host.end());
    LPCWSTR sw = stemp.c_str();

    SendSockAddr.sin_family = AF_INET;
    SendSockAddr.sin_port = htons(port+1);

    RecvSockAddr.sin_family = AF_INET;
    RecvSockAddr.sin_port = htons(port);

    InetPton(AF_INET, sw, &RecvSockAddr.sin_addr.s_addr);
    InetPton(AF_INET, sw, &SendSockAddr.sin_addr.s_addr);

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
        cout << msg.size() << " " << FinMessage.value[0].size() << endl;
        if (msg.substr(4, msg.size()).size() < FinMessage.value[0].size()) {

            cout << "RECIVED LAST MESSAGE" << endl;
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

int Initialize(string host, int port) {

    int ID = -1;


    if (WIN) {
        InitializeWinSock();
    }

    InitializeSocket();
    BuildSocket(host, port);


    return 0;

}

string Recive(string host, int port) {
    Initialize(host, port);
    string ans;
    Message X = ReciveWithoutInit();
    for (int i = 0; i < 1000; i++) {
        if (X.value[i] != "0") {
            ans += X.value[i];
        }
    }
    cout << "co";

    cout << ans;
    return ans;
}



*/
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
char RecvBufferedMsg[BUFF];

class reciveconnection {
private:
    SOCKET SendSock = INVALID_SOCKET;
    SOCKET RecvSock = INVALID_SOCKET;

    struct sockaddr_in SendSockAddr;
    struct sockaddr_in RecvSockAddr;

    int port;
    int timeout;
    int NumberOfPackets;

    string host;

    struct Message {
        string value[1000];
    };

    bool RecivedPackets[1000] = { 0 };

    //SOCKET INITLIALIZATION 

    //initilazaing winsock
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

    void adress_sockets() {
        SendSockAddr.sin_family = AF_INET;
        SendSockAddr.sin_port = htons(port+1);

        RecvSockAddr.sin_family = AF_INET;
        RecvSockAddr.sin_port = htons(port);

    }

    void convert_sockets_to_bin() {

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
    int BuildSocket() {

        adress_sockets();
        convert_sockets_to_bin();
        return attempt_biniding_recive_sock();

    }

    int CloseSocket() {

        closesocket(SendSock);
        closesocket(RecvSock);
        WSACleanup();
        return 1;
    }

    int Initialize() {

        if (WIN) {
            InitializeWinSock();
        }


        if (InitializeSocket() || BuildSocket()) {
            printf("aborting process due to above error");
            return 1;
        }


        return 0;
    }


    // Functions to recive data from client 

    void is_message_valid(int recived) {
        if (recived == SOCKET_ERROR) {
            printf("// recvfrom failed with error %d\n", WSAGetLastError());
            abort();
        }
    }

    string single_packet_recive() {
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

    


    //Functions to Send

    bool is_socket_error(int result) {
        if (result == SOCKET_ERROR) {
            printf("// while sending got an error: %d\n", WSAGetLastError());
            return 1;
        }

        return 0;
    }

    int send_single_packet(std::string x) {



        for (int i = 0; i < x.length(); i++) {
            SendMsgBuffered[i] = x[i];
        }

        int result = sendto(SendSock, SendMsgBuffered, x.length(), 0, (SOCKADDR*)&SendSockAddr, sizeof(SendSockAddr));

        return is_socket_error(result);
    }



    string recive_and_resend() {
        /// 
        /// recives data resend if needed
        /// 
        
        
        int Recived = 0;
        string msg;

        int FinMessage[10000] = {0};


        int ID;
        int LastID = -2;
        int size = 0;

        string ans = "";

        while (1 == 1) {
            Recived = 0;
            msg = single_packet_recive();


            if (msg.substr(1, 3) == "000") {
                ID = 0;
                ans += msg.substr(3, msg.size());
            }
            else {
                std::stringstream str(msg.substr(0, 3));
                str >> ID;
                ans += msg.substr(3, msg.size());

            }


            send_single_packet(to_string(ID));
            
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

        return ans;

    }



public:
    reciveconnection(string userhost, int userport, int usertimeout = 1) {
        host = userhost;
        port = userport;
        timeout = usertimeout;
        Initialize();

    }



    string Recive() {
        
        string ans = recive_and_resend();

        CloseSocket();

        return ans;
    }

};







string Recive(string host, int port) {
    reciveconnection x = reciveconnection(host, port);
    return x.Recive();
    }
