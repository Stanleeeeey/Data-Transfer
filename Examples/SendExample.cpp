#include <iostream>
#include "SERVERPROTO.hpp"
#include <thread>
#include <chrono>






int main() {
    string x;

    for (int i = 0; i < 30000; i++) {
        x = x + "Aa";
    }


    int y = Send(x, "127.0.0.1", 1234, 10000);

    
    cout << "Sended " << endl;


    return EXIT_SUCCESS;



}
