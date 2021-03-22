# chat_client
Chat program based on TCPv6 sockets

## Project descripton
This application was an assignment for a class at university. It is by no means bug-free. 

## How to use
The server should be started before the client.

Start the client in this manner: Client.exe IPv6 Serverport name

If more arguments than IPv6, port and name are passed, those that are too many are ignored.

If there are too few arguments, the application terminates itself.

It is indicated if the client has connected successfully. He can then send the first message.
