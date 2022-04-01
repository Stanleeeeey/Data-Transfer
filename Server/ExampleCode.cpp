#include <iostream>
#include "SERVERPROTO.hpp"






int main() {
    string x;

    for (int i = 0; i < 30000; i++) {
        x = x + "Aa";
    }


    Send(x);

    cout << "Sended " << endl;


    return EXIT_SUCCESS;



}
