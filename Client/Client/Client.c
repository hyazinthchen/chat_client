//A simple client application using TCPv6-Sockets
#define _CRT_SECURE_NO_WARNINGS
#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Winsock2.h>
#include <ws2tcpip.h>

#define DEFAULT_FAMILY      AF_INET6    //Protocol family - in this case force IPv6
#define DEFAULT_SOCKTYPE    SOCK_STREAM //TCP uses SOCK_STREAM, UDP uses SOCK_DGRAM
#define DEFAULT_PORT        "1234"      //The port for testing
#define BUFFER_SIZE         1024        //The buffer size for the demonstration

//data structure that will be sent to the server
struct package {
    char sNumber[7];
    char txt[BUFFER_SIZE];
}; 

char* init_package(struct package p) {
    char* package_ZK = NULL;
    int sNumberLength = strlen(p.sNumber);
    int txtLength = strlen(p.txt);
    int totalLength = sNumberLength + txtLength - 1;

    package_ZK = (char*)malloc(totalLength * sizeof(char)); //allocate memory for the package

    if (package_ZK == NULL) {
        printf("Error on building package.\n");
        EXIT_FAILURE;
    }

    int i, j;

    for (i = 0; i < sNumberLength; i++) {
        package_ZK[i] = p.sNumber[i];
    }

    for (j = 0, i = 0; j <= txtLength; i++, j++) {
        package_ZK[i] = p.txt[j];
    }

    return package_ZK;
}

int main(int argc, char* argv[]) {
    char receivebuffer[BUFFER_SIZE];
    char sendbuffer[BUFFER_SIZE];
    SOCKET ClientSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, hints;
    int iResult;
    int bufferLength = BUFFER_SIZE;

    //if (argc < 3) {
    //    fprintf(stderr, "Application needs IP adress, sNumber and portnumber as arguments.\n");
    //    exit(1);
    //}

    const char* ip = argv[1];
    const char* port = argv[2];
    const char* sNumber = argv[3];

    //initialise TCP for Windows ("Winsock")
    WORD wVersionRequested; //WORD = unsigned long data type, version of the Winsocket
    WSADATA wsaData; //stores information about the Winsocket implementation
    wVersionRequested = MAKEWORD(2, 2); //MAKEWORD = macro to request version 2.2

    iResult = WSAStartup(wVersionRequested, &wsaData); //makes it possible for the process to run WINSOCK.DLL
    if (iResult != 0) {
        printf("Error on initialising Winsock.\n");
        exit(1);
    }
    else {
        printf("Winsock initialised.\n");
    }

    ZeroMemory(&hints, sizeof(hints)); //Macro to fill a block of memory with zeros
    hints.ai_family = DEFAULT_FAMILY;
    hints.ai_socktype = DEFAULT_SOCKTYPE;
    hints.ai_protocol = IPPROTO_TCP; //specifies the TCP protocol

    iResult = getaddrinfo("::1", DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        exit(1);
    }

    //create a socket that will connect to the server
    ClientSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (ClientSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        exit(1);
    }

    //connect to the server
    iResult = connect(ClientSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ClientSocket);
        ClientSocket = INVALID_SOCKET;
    }

    //free the address information that getaddrinfo has returned
    freeaddrinfo(result);

    if (ClientSocket == INVALID_SOCKET) {
        printf("Unable to connect to server.\n");
        WSACleanup();
        exit(1);
    }

    printf("You are now connected.\n");

    while (1) {
        gets(sendbuffer);

        //-------------------------------TODO: send package instead of sendbuffer to the server-------------------------------
        //struct package p; //build new data package
        //strcpy(p.sNumber, sNumber); //fill data package with data
        //strcpy(p.txt, sendbuffer);

        //char* package_ZK = NULL;
        //package_ZK = init_package(p); //pointer to the built data package


        iResult = send(ClientSocket, sendbuffer, (int)strlen(sendbuffer), 0);
        if (iResult == 0 || iResult == SOCKET_ERROR) {
            printf("Server has disconnected.\n");
            closesocket(ClientSocket);
            WSACleanup();
            exit(1);
        }
        iResult = recv(ClientSocket, receivebuffer, bufferLength, 0);
        if (iResult > 0) {
            receivebuffer[iResult] = '\0';
            printf("Message: %s\n", receivebuffer);
        }
    }
    closesocket(ClientSocket);
    WSACleanup();
    return 0;
}