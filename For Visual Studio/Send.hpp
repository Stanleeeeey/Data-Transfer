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

// global variables
#define BUFF 10
#define BUFFID 3


atomic<bool> ThreadRunFlag(false);
char SendMsgBuffered[BUFF + BUFFID];

class SendSockets {
    private:


        string host = "";
        int port    = NULL;

        int initialize_WinSock() {

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

        void init_sockets() {
            SendSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
            RecvSock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
        }

        int initialize_socket() {

            init_sockets();

            return check_if_socket_valid();

        }

        void adress_sockets() {
            SendSockAddr.sin_family = AF_INET;
            SendSockAddr.sin_port = htons(port);

            RecvSockAddr.sin_family = AF_INET;
            RecvSockAddr.sin_port = htons(port + 1);

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
        int build_socket() {

            adress_sockets();
            convert_sockets_to_bin();
            return attempt_biniding_recive_sock();

        }



    public:
        SOCKET SendSock = INVALID_SOCKET;
        SOCKET RecvSock = INVALID_SOCKET;

        struct sockaddr_in SendSockAddr = { 0 };
        struct sockaddr_in RecvSockAddr = { 0 };

        bool initalize(int userport, string userhost) {
            port = userport;
            host = userhost;

            if (WIN) {
                initialize_WinSock();
            }


            if (initialize_socket() || build_socket()) {
                printf("aborting process due to above error");
                return false;
            }
            return true;

        }

        int close_socket() {

            closesocket(SendSock);
            closesocket(RecvSock);
            WSACleanup();
            return 1;
        }

};



class SendConnection {
    private: 

        int timeout;            // time after program should resend message
        int NumberOfPackets;    // Number of sended messages

        string Message[1000];   

        SendSockets *ServerSockets = new SendSockets();
        
        bool RecivedPackets[1000] = { 0 };

        // Functions to recive data from client 
        int convert_to_int(string msg) {
            //
            // converts id to int
            // 
            int x;

            std::stringstream str(msg);
            str >> x;
            
            return x;
        }

        void is_message_valid(int recived) {
            //
            //  ckecks if message is valid
            //
            if (recived == SOCKET_ERROR) {
                printf("// recvfrom failed with error %d\n", WSAGetLastError());
                
            }
        }

        tuple<string, int> recive_single_packet() {
            //
            //  recives single packet
            //
            char RecvBufferedMsg[BUFFID];

            int ServerAddrSize = sizeof(ServerSockets->RecvSockAddr);

            int recived = recvfrom(ServerSockets->RecvSock, RecvBufferedMsg, BUFFID, 0, (SOCKADDR*)&ServerSockets->RecvSockAddr, &ServerAddrSize);

            return { RecvBufferedMsg, recived };
        }

        int get_id_packet() {
            //
            //  get id of the packet
            //
            string data_pack;
            int recived;

            tie(data_pack, recived) = recive_single_packet();
            is_message_valid(recived);

            return convert_to_int(data_pack);

        }

        int find_and_resend_missing_packets(int ID) {
            //
            //  finds missing packets and resend them
            //

            int Failed = 0;
            for (int i = 0; i < ID + 1; i++) {
                if (!RecivedPackets[i]) {

                    Failed++;

                    send_single_packet(Message[i]);
                }
            }

            return Failed;
        }

        bool is_last_id(int ID) {
            //
            //  checks if the packet is the last one
            //
            if (ID + 1 == NumberOfPackets) {
                return true;
            }
            return false;
        }

        void ReciveSendingSide() {
            //
            //  checks if everything was recived
            //

            int ID;

            std::fill(std::begin(RecivedPackets), std::end(RecivedPackets), false);

            printf("////    Recive Thread Succesfully Started\n");

            while (1 == 1) {

                ID = get_id_packet();

                RecivedPackets[ID] = true;

                if (find_and_resend_missing_packets(ID)==0 && is_last_id(ID)) {
                    ThreadRunFlag = true;
                    cout << "END" << endl;

                }
            }
        }


        //Functions to Send

        bool is_socket_error(int result) {
            //
            //  check if there is a socket error
            //
            if (result == SOCKET_ERROR) {
                printf("// while sending got an error: %d\n", WSAGetLastError());
                return 1;
            }

            return 0;
        }
        
        int send_single_packet(std::string x) {
            //
            //  sends a single packet to the client
            //
            for (int i = 0; i < x.length(); i++) {
                SendMsgBuffered[i] = x[i];
            }

            int result = sendto(ServerSockets->SendSock, SendMsgBuffered, (int)x.length(), 0, (SOCKADDR*)&ServerSockets->SendSockAddr, sizeof(ServerSockets->SendSockAddr));

            return is_socket_error(result);
        }

        void SendWithoutChecking() {
            //
            //  sends message to the client
            //
            printf("////    Send Thread succesfully started\n");


            for (int i = 0; i < NumberOfPackets; i++) {

                send_single_packet(Message[i]);
                

            }
            
            cout << "////     Ended Sending Thread" << endl;



        }
        

        //timer functions

        void wait() {
            //
            //  waits untill timeout passes
            //
            std::chrono::milliseconds timespan(timeout);

            std::this_thread::sleep_for(timespan);
        }

        void Timer() {
            //
            //  Sends message again after timeout
            //
            wait();
            while (ThreadRunFlag == false) {
                cout << "timeout resending" << endl;
                SendWithoutChecking();
                wait();
            }


        }

        tuple<int, int> increase_buff(int msgsize) {
            //
            //  if needed increases size of the packet
            //
            int LocalBUFF = BUFF;
            int length = (int)ceil((msgsize) * 1.0 / (LocalBUFF - 4));

            while (length > 999 || length % LocalBUFF == 0) {
                cout << "Increasing Buff Size ACTUAL BUFF SIZE:" << LocalBUFF + 1 << endl;

                LocalBUFF++;
                length = (int)ceil((msgsize) * 1.0 / (LocalBUFF - 4));
            }

            return { LocalBUFF, length };
        }

        string id_creation(int id) {
            //
            //  create id in format 001
            //
            stringstream ss;

            ss << setw(3) << setfill('0') << (id) % 1000;
            return ss.str();
        }

        void IntoBatches(string message) {
            //
            //  Converts string inton list of smalller strings (packets)
            //


            
            string part;

            int IdCount = BUFFID;
            int LocalBUFF, length;
            
            tie( LocalBUFF, length ) = increase_buff((int)message.length());

            NumberOfPackets = length;

            for (int i = 0; i < length; i++) {

                string s = id_creation(i);

                if (message.length() >= (LocalBUFF - 4)) {

                    Message[i] = s + message.substr(0, LocalBUFF - 4);
                    message = message.substr(LocalBUFF - 4, message.length());


                }
                else {

                    Message[i] = s + message;

                }
            }
        }



        void runthreads() {
            //
            //  Runs Recive and timeout simultaneously
            //
            vector<std::thread> threads;
            threads.push_back(std::thread(&SendConnection::ReciveSendingSide, this));
            threads.push_back(std::thread(&SendConnection::Timer, this));

            for (auto& thread : threads) {
                thread.join();
            }
        }

    public:
        SendConnection(string userhost, int userport, int usertimeout=1) {
            //
            //  Initiliazing class of SendConnection
            //
            //  userhost:    host of the client as string
            //  userport:    port of the client as int
            //  usertimeout: time aftre program resend message

            timeout = usertimeout;

            ServerSockets->initalize(userport, userhost);


        }



        void Send(string message) {
            //
            //  send message to the client
            //
            IntoBatches(message);
            SendWithoutChecking();
            runthreads();
            
            ServerSockets->close_socket();
        }

};


int Send(string msg, string host, int port, int timeout) {
    // 
    //     Function to send data to client
    // 
    // msg:     string to send
    // host:    string describing host to listen ex. "127.0.01"
    // port:    int describing port to listen
    // timeout: time in ms after program should resend data
   
    cout << "//Starting Winsock Initialize and Socket Build" << endl;
    SendConnection *sendconnection = new SendConnection(host, port, timeout);

    sendconnection->Send(msg);

    return 0;

}