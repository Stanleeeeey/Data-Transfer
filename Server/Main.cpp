#include <iostream>
#include "SERVERPROTO.hpp"
#include <thread>
#include <chrono>



int InitProt(string msg, int type, int ){

    IntoBatches(msg, type);

    struct Send_args args;

    for (int i = 0; i < 1000; i++) {
        if (Batched[i] == "") {
            break;
        }
        args.Batched[i] = Batched[i];
    }
    


    std::thread SendingThread(Send, args);
    SendingThread.join();

    std::thread ReciveThread(Recive, args);
    ReciveThread.join();

    return 0;

}


int main(){
    string x;
    
    for(int i = 0; i<15000;i++){
        x = x + "Aa";
    }
    cout<<"initialize"<<endl;
    Initialize();


    InitProt(x, 0 , 0);

    cout << "Sended " << endl;

    return 0;

}
