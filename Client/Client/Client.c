//A simple client application using TCPv6-Sockets
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Winsock2.h>
#include <ws2tcpip.h>
#include "Packet.h"

#define DEFAULT_FAMILY      AF_INET6    //Protocol family - in this case force IPv6
#define DEFAULT_SOCKTYPE    SOCK_STREAM //TCP uses SOCK_STREAM, UDP uses SOCK_DGRAM

//initialise TCP for Windows ("Winsock")
void initialiseWinsock() {
    int returnValue;

    WORD wVersionRequested; //WORD = unsigned long data type, version of the Winsocket
    WSADATA wsaData; //stores information about the Winsocket implementation
    wVersionRequested = MAKEWORD(2, 2); //MAKEWORD = macro to request version 2.2

    returnValue = WSAStartup(wVersionRequested, &wsaData); //makes it possible for the process to run WINSOCK.DLL
    if (returnValue != 0) {
        printf("Error on initialising Winsock.\n");
        exit(1);
    }
    else {
        printf("Winsock initialised.\n");
    }
}

int main(int argc, char* argv[]) {
    SOCKET clientSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, hints;
    int returnValue;
    struct Packet message_cli;
    struct Packet message_serv;
   
    if (argc < 4) {
        fprintf(stderr, "Application needs IP adress, port and name as arguments.\nUsage: Client.exe ::1 1234 s12345\n");
        exit(1);
    }
    if (argc > 4) {
        fprintf(stderr, "You have entered too many arguments. This application only accepts 3 arguments. Anything else will be ignored.\n");
    }

    //initalise datastruct
    strcpy(message_cli.text, " ");
    strcpy(message_cli.name, argv[3]);
    
    const char* ip = argv[1];
    const char* port = argv[2];

    initialiseWinsock();

    ZeroMemory(&hints, sizeof(hints)); //Macro to fill a block of memory with zeros
    hints.ai_family = DEFAULT_FAMILY;
    hints.ai_socktype = DEFAULT_SOCKTYPE;
    hints.ai_protocol = IPPROTO_TCP; //specifies the TCP protocol

    returnValue = getaddrinfo(ip, port, &hints, &result);
    if (returnValue != 0) {
        printf("getaddrinfo failed: %d\n", returnValue);
        WSACleanup();
        exit(1);
    }

    //create a socket that will connect to the server
    clientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (clientSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        exit(1);
    }

    //connect to the server
    returnValue = connect(clientSocket, result->ai_addr, (int)result->ai_addrlen);
    if (returnValue == SOCKET_ERROR) {
        closesocket(clientSocket);
        clientSocket = INVALID_SOCKET;
    }

    //free the address information that getaddrinfo has returned
    freeaddrinfo(result);

    if (clientSocket == INVALID_SOCKET) {
        printf("Unable to connect to server.\n");
        WSACleanup();
        exit(1);
    }

    printf("You are now connected.\n");

    while (1) {
        printf("%s> ", &message_cli.name);
        gets(&message_cli.text);
        returnValue = send(clientSocket, &message_cli, sizeof(message_cli), 0);
        if (returnValue == 0 || returnValue == SOCKET_ERROR) {
            printf("Server has disconnected.\n");
            closesocket(clientSocket);
            WSACleanup();
            exit(1);
        }

        returnValue = recv(clientSocket, &message_serv, sizeof(message_serv), 0);
        if (returnValue > 0) {
            printf("%s> %s\n",message_serv.name, message_serv.text);
        }
    }

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}