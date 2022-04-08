#include <iostream>
#include "Send.hpp"

using namespace std;

int main(){
    std::cout<< "works"<<std::endl;
    Send("aaaa", "127.0.0.1", 5555, 1000);
    return 0;
}