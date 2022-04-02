# Data-Transfer

Simple code for sending string messages,

# HOW TO USE IT

firsrt download [SERVERPROTO.hpp](https://github.com/mastercode5/Data-Transfer/blob/main/Server/SERVERPROTO.hpp) (used for sending messages) and [CLIENTPROTO.hpp](https://github.com/mastercode5/Data-Transfer/blob/main/Client/CLIENTPROTO.hpp) (used for reciving).
<br><br>
then simply include files in your code (`SERVERPROTO` for Sending or `CLIENTPROTO` for reciving) 
<br><br>
WARNING you have to start first reciving part of code 
<br><br>
then simply in your reciving code use `Recive(string host, int port)` whisch returns string with a message
<br><br>
and `Send(string message, string host, int port)` in the Sending part of your code.
<br><br>
There are also two examples of using those functions [one for a Recive()](https://github.com/mastercode5/Data-Transfer/blob/main/Client/ExampleClient.cpp) and [example of using Send()](https://github.com/mastercode5/Data-Transfer/blob/main/Server/ExampleCode.cpp)

Hope this code will be useful.
