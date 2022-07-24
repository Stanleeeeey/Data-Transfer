# Data-Transfer

Simple Headerfile library for sending string message using winsock.

**Quick note:** this code need optimization

# HOW TO USE IT

Warning!! the easiest way to work with the library is to use visual studio compiler, gcc still don't support timeout (still requires it as argument)

## Installation and configuration
first download `Send.hpp` ([for visual studio](https://github.com/mastercode5/Data-Transfer/blob/main/For%20Visual%20Studio/Send.hpp), or [for gcc compiler](https://github.com/mastercode5/Data-Transfer/blob/main/For%20GCC%20compiler/Send.hpp)) (used for sending messages) and `Recive.hpp` ([for visual studio](https://github.com/mastercode5/Data-Transfer/blob/main/For%20Visual%20Studio/Recive.hpp), or [for gcc compiler](https://github.com/mastercode5/Data-Transfer/blob/main/For%20GCC%20compiler/Recive.hpp)) (used for reciving). Move them to the folder with your code, then `#include` them (`#include "Recive.hpp"` and `#include "Send.hpp"`). Use Send() and Recive() functions. 

## Send() function
file for visual studio : [Send.hpp](https://github.com/mastercode5/Data-Transfer/blob/main/For%20Visual%20Studio/Send.hpp)<br>
file for gcc compiler  : [Send.hpp](https://github.com/mastercode5/Data-Transfer/blob/main/For%20GCC%20compiler/Send.hpp)

Syntax : ```Send(string msg, string host, int port, int timeout)```

Example of use : [here](https://github.com/Stanleeeeey/Data-Transfer/blob/main/Examples/SendExample.cpp)
### Arguments
|Argument|Description|
|--------|-----------|
|`message` |string to send|
|`host`| host adrress as a string|
|`port`|port adress as a int|
|`timeout`|time in miliseconds after program will resend data|

## Recive() function 
file for visual studio : [Recive.hpp](https://github.com/mastercode5/Data-Transfer/blob/main/Client/Recive.hpp)<br>
file for gcc compiler  : [Recive.hpp](https://github.com/mastercode5/Data-Transfer/blob/main/For%20GCC%20compiler/Recive.hpp)

Syntax : ```Recive(string host, int port)```

Example of use : [here](https://github.com/Stanleeeeey/Data-Transfer/blob/main/Examples/ReciveExample.cpp)

### Arguments
|Argument|Description|
|--------|-----------|
|`host`|host adress as a string|
|`port`|port adress as a int|



# HOW IT WORKS

## SENDER

1. We get the string x to send to the client.
2. We split x into packets with id
3. We send every packet
4. We recive ids from the reciver
5. if any ID is missing we resend packet with that id untill we recive this id back
## RECIVER
1.  We recive packet with ID
2.  Ans[ID] = packet
3.  We resend ID
4.  recive new messages while all packets aren't recived

![image](https://user-images.githubusercontent.com/43855236/174836396-0afddf35-0464-45a3-ac96-afdec996a5b2.png)





Hope this code will be useful.
