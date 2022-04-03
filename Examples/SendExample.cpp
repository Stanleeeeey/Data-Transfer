#include <iostream>
#include "CLIENTPROTO.h"



int main() {
	string x = Recive("127.0.0.1", 1234);
	cout << x << endl;
	return 0;

}
