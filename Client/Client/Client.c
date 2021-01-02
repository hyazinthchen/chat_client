#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <winsock.h>
#include <io.h>
#pragma comment(lib, "Ws2_32.lib")

#define PORT 1234
#define RCVBUFSIZE 8192

/* Funktion gibt aufgetretenen Fehler aus und
 * beendet die Anwendung. */
static void error_exit(char* errorMessage) {

    fprintf(stderr, "%s: %d\n", errorMessage, WSAGetLastError());
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    struct sockaddr_in server;
    struct hostent* host_info;
    unsigned long addr;
    SOCKET sock;
    char* echo_string;
    int echo_len;

    /* Initialisiere TCP f�r Windows ("winsock"). */
    WORD wVersionRequested;
    WSADATA wsaData;
    wVersionRequested = MAKEWORD(1, 1);
    if (WSAStartup(wVersionRequested, &wsaData) != 0)
        error_exit("Fehler beim Initialisieren von Winsock");
    else
        printf("Winsock initialisiert\n");

    /* Sind die erforderlichen Kommandozeilenargumente vorhanden? */
    if (argc < 3)
        error_exit("usage: client server-ip echo_word\n");

    /* Erzeuge das Socket. */
    sock = socket(AF_INET, SOCK_STREAM, 0);


    if (sock < 0)
        error_exit("Fehler beim Anlegen eines Sockets");

    /* Erzeuge die Socketadresse des Servers.
     * Sie besteht aus Typ, IP-Adresse und Portnummer. */
    memset(&server, 0, sizeof(server));
    if ((addr = inet_addr(argv[1])) != INADDR_NONE) {
        /* argv[1] ist eine numerische IP-Adresse. */
        memcpy((char*)&server.sin_addr, &addr, sizeof(addr));
    }
    else {
        /* F�r den Fall der F�lle: Wandle den
         * Servernamen bspw. "localhost" in eine IP-Adresse um. */
        host_info = gethostbyname(argv[1]);
        if (NULL == host_info)
            error_exit("Unbekannter Server");
        /* Server-IP-Adresse */
        memcpy((char*)&server.sin_addr,
            host_info->h_addr, host_info->h_length);
    }
    /* IPv4-Verbindung */
    server.sin_family = AF_INET;
    /* Portnummer */
    server.sin_port = htons(PORT);

    /* Baue die Verbindung zum Server auf. */
    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) < 0)
        error_exit("Kann keine Verbindung zum "
            "Server herstellen");

    /* Zweites Argument wird als "echo" beim Server verwendet. */
    echo_string = argv[2];
    /* L�nge der Eingabe */
    echo_len = strlen(echo_string);

    /* den String inkl. Nullterminator an den Server senden */
    if (send(sock, echo_string, echo_len, 0) != echo_len)
        error_exit("send() hat eine andere Anzahl"
            " von Bytes versendet als erwartet !!!!");


    /* Schlie�e Verbindung und Socket. */
    closesocket(sock);
    /* Cleanup Winsock */
    WSACleanup();
    return EXIT_SUCCESS;
}