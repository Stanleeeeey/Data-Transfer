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
#define BUFF 10
#define BUFFID 3

atomic<bool> ThreadRunFlag(false);
char SendMsgBuffered[BUFF + BUFFID];

class SendConnection {
    private:
        SOCKET SendSock = INVALID_SOCKET;
        SOCKET RecvSock = INVALID_SOCKET;

        struct sockaddr_in SendSockAddr;
        struct sockaddr_in RecvSockAddr;
        



        int port;
        int timeout;
        int NumberOfPackets;

        string host;
        string Message[1000];
        
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


            if (InitializeSocket() || BuildSocket()){
                printf("aborting process due to above error");
                return 1;
            }


            return 0;
        }


        // Functions to recive data from client 
        int convert_to_int(string msg) {
            /// 
            ///     return id of the packet x
            ///
            int x;
            if (msg == "000") {
                x = 0;
            }
            else {
                std::stringstream str(msg);
                str >> x;
            }

            return x;
        }

        void is_message_valid(int recived) {
            ///
            ///     checks if there is no error in recived packet
            ///
            if (recived == SOCKET_ERROR) {
                printf("// recvfrom failed with error %d\n", WSAGetLastError());
                abort();
            }
        }

        int single_packet_recive() {
            ///
            ///     recive single packet 
            ///
            ///     returns: if of the packet
            char RecvBufferedMsg[BUFFID];

            int ServerAddrSize = sizeof(RecvSockAddr);

            int recived = recvfrom(RecvSock, RecvBufferedMsg, BUFFID, 0, (SOCKADDR*)&RecvSockAddr, &ServerAddrSize);

            is_message_valid(recived);

            return convert_to_int(RecvBufferedMsg);
        }

        int find_and_resend_missing_packets(int ID) {
            ///
            ///     resends packet if needed
            ///
            /// ID : current packet id
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
            ///
            ///     checks if  ID is the id of the last packet
            ///
            /// ID : int to check
            if (ID + 1 == NumberOfPackets) {
                return true;
            }
            return false;
        }

        void ReciveSendingSide() {
            ///
            ///     Recives packets ids from client resend if needed
            ///

            int ID;

            std::fill(std::begin(RecivedPackets), std::end(RecivedPackets), false);

            printf("////    Recive Thread Succesfully Started\n");

            while (1 == 1) {

                ID = single_packet_recive();

                RecivedPackets[ID] = true;

                if (find_and_resend_missing_packets(ID)==0 && is_last_id(ID)) {
                    ThreadRunFlag = true;
                    cout << "END" << endl;

                }
            }
        }


        //Functions to Send

        bool is_socket_error(int result) {
            ///
            ///     checks if error occured while sending packet
            ///
            if (result == SOCKET_ERROR) {
                printf("// while sending got an error: %d\n", WSAGetLastError());
                return 1;
            }

            return 0;
        }
        
        int send_single_packet(std::string x) {
            ///
            ///     sends message x to the client
            ///

            for (int i = 0; i < x.length(); i++) {
                SendMsgBuffered[i] = x[i];
            }

            int result = sendto(SendSock, SendMsgBuffered, x.length(), 0, (SOCKADDR*)&SendSockAddr, sizeof(SendSockAddr));

            return is_socket_error(result);
        }

        void SendWithoutChecking() {
            ///
            ///     Sends all of the packets
            ///

            printf("////    Send Thread succesfully started\n");


            for (int i = 0; i < NumberOfPackets; i++) {

                send_single_packet(Message[i]);

            }

            cout << "////     Ended Sending Thread" << endl;



        }
        

        //timer functions

        void wait() {
            ///
            /// wait timeout (miliseconds)
            ///
            std::chrono::milliseconds timespan(timeout);

            std::this_thread::sleep_for(timespan);
        }
        void Timer() {
            ///
            ///     Resends message after timeout 
            ///
            wait();
            while (ThreadRunFlag == false) {
                cout << "timeout resending" << endl;
                SendWithoutChecking();
                wait();
            }


        }

        tuple<int, int> increase_buff(int msgsize) {
            ///
            ///     adjust BUFFER size to messagesize
            ///
            /// msgsize: size of message to send

            int LocalBUFF = BUFF;
            int length = ceil((msgsize) * 1.0 / (LocalBUFF - 4));

            while (length > 999 || length % LocalBUFF == 0) {
                cout << "Increasing Buff Size ACTUAL BUFF SIZE:" << LocalBUFF + 1 << endl;

                LocalBUFF++;
                length = ceil((msgsize) * 1.0 / (LocalBUFF - 4));
            }

            return { LocalBUFF, length };
        }

        string id_creation(int id) {
            ///
            ///     convert int x to form 00x
            ///

            stringstream ss;

            ss << setw(3) << setfill('0') << (id) % 1000;
            return ss.str();
        }

        void IntoBatches(string message) {

            ///
            ///     convert string to packets stored in Message[]
            ///
            ///     msg: string with data to process

            int LocalBUFF, length;
            
            tie( LocalBUFF, length ) = increase_buff(message.size());

            NumberOfPackets = length; //remember number of packets

            for (int i = 0; i < length; i++) {

                string s = id_creation(i); // convert int x to form 00x

                if (message.size() >= (LocalBUFF - 4)) {

                    Message[i] = s + message.substr(0, LocalBUFF - 4);
                    message = message.substr(LocalBUFF - 4, message.size());


                }
                else {
                    //if last
                    Message[i] = s + message;

                }
            }
        }



        void runthreads() {
            ///
            ///   run threads for checikng if delivered and start timer
            ///
            
            vector<std::thread> threads;

            threads.push_back(std::thread(&SendConnection::ReciveSendingSide, this)); // thread for checking if delivered
            threads.push_back(std::thread(&SendConnection::Timer, this));             // timer thread

            //run threads
            for (auto& thread : threads) {
                thread.join();
            }
        }

    public:
        SendConnection(string userhost, int userport, int usertimeout=1) {
            host = userhost;
            port = userport;
            timeout = usertimeout;
            Initialize(); //initialize sockets

        }



        void Send(string message) {

            IntoBatches(message);  // convert message int o packets
            SendWithoutChecking(); // send packets
            runthreads();          // (in threads) check if data delivered and run timeout
            CloseSocket();         // close sockets
        }

};


int Send(string msg, string host, int port, int timeout) {
    /// 
    ///     Function to send data to client
    /// 
    /// msg     : string to send</param>
    /// host    : string describing host to listen ex. "127.0.01"</param>
    /// port    : int describing port to listen</param>
    /// timeout : time in ms after program should resend data</param>
   
    cout << "//Starting Winsock Initialize and Socket Build" << endl;
    SendConnection sendconnection = SendConnection(host, port, timeout);

    sendconnection.Send(msg);




    return 0;

}