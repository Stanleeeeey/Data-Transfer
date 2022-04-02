# Data-Transfer

Simple Headerfile library for sending string message using winsock.

# HOW TO USE IT

firsrt download [Send.hpp](https://github.com/mastercode5/Data-Transfer/blob/main/Server/Send.hpp) (used for sending messages) and [Recive.hpp](https://github.com/mastercode5/Data-Transfer/blob/main/Client/Recive.hpp) (used for reciving). Move them to the folder eith you code, then `#include` them (`#include Recive.hpp` and `#include Send.hpp` and use Send() and Recive() functions.

## Send() function
file : [Send.hpp](https://github.com/mastercode5/Data-Transfer/blob/main/Server/Send.hpp)

Syntax : ```cpp Send(string msg, string host, int port, int timeout)```

Example of use : [here](https://github.com/mastercode5/Data-Transfer/blob/main/Server/ExampleCode.cpp)

### Arguments
|Argument|Description|
|--------|-----------|
|`message` |string to send|
|`host`| host adrress as a string|
|`port`|port adress as a int|
|`timeout`|time in miliseconds after program will resend data|

## Recive() function 
from file : [Recive.hpp](https://github.com/mastercode5/Data-Transfer/blob/main/Client/Recive.hpp)

Syntax : ```cpp Recive(string host, int port)```

Example of use : [here](https://github.com/mastercode5/Data-Transfer/blob/main/Client/ExampleClient.cpp)

### Arguments
|Argument|Description|
|--------|-----------|
|`host`|host adress as a string|
|`port`|port adress as a int|





Hope this code will be useful.
