#pragma comment(lib, "Ws2_32.lib")
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Winsock2.h>
#include <ws2tcpip.h>


#define DEFAULT_FAMILY      AF_INET6    // Protocol family - in this case force IPv6
#define DEFAULT_SOCKTYPE    SOCK_STREAM // TCP uses SOCK_STREAM, UDP uses SOCK_DGRAM
#define DEFAULT_PORT        "1234"      // The port for testing
#define BUFFER_SIZE         1024        // The buffer size for the demonstration

int main(int argc, char* argv[]) {
    char buffer[BUFFER_SIZE];
    SOCKET ClientSocket = INVALID_SOCKET;
    struct addrinfo* result = NULL, * ptr = NULL, hints;
    int iResult;
    int recvbuflen = BUFFER_SIZE;
    const char* sendbuf = "this is a test";

    // Initialise TCP for Windows ("Winsock").
    WORD wVersionRequested; //WORD = unsigned long data type, version of the Winsocket
    WSADATA wsaData; // stores information about the Winsocket implementation
    wVersionRequested = MAKEWORD(2, 2); //MAKEWORD = macro to request version 2.2

    iResult = WSAStartup(wVersionRequested, &wsaData); //makes it possible for the process to run WINSOCK.DLL
    if (iResult != 0) {
        printf("\nError on initialising Winsock.\n");
        exit(1);
    }
    else {
        printf("\nWinsock initialised.\n");
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

    // Attempt to connect to the first address returned by
    // the call to getaddrinfo
    ptr = result;

    // Create a SOCKET for connecting to server
    ClientSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (ClientSocket == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Connect to server.
    iResult = connect(ClientSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        closesocket(ClientSocket);
        ClientSocket = INVALID_SOCKET;
    }

    // Should really try the next address returned by getaddrinfo
    // if the connect call failed
    // But for this simple example we just free the resources
    // returned by getaddrinfo and print an error message

    freeaddrinfo(result);

    if (ClientSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }

    // Send an initial buffer
    iResult = send(ClientSocket, sendbuf, (int)strlen(sendbuf), 0);
    if (iResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    printf("Bytes Sent: %ld\n", iResult);

    // shutdown the connection for sending since no more data will be sent
    // the client can still use the ConnectSocket for receiving data
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // Receive data until the server closes the connection
    do {
        iResult = recv(ClientSocket, buffer, recvbuflen, 0);
        if (iResult > 0)
            printf("Bytes received: %d\n", iResult);
        else if (iResult == 0)
            printf("Connection closed\n");
        else
            printf("recv failed: %d\n", WSAGetLastError());
    } while (iResult > 0);

    // shutdown the send half of the connection since no more data will be sent
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}