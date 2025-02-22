#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include <iostream>

#include "./UserData/UserData.h"

#pragma comment(lib, "Ws2_32.lib")

#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER 512

char recBuf[DEFAULT_BUFFER];
int recResult, recSendResult;
const int recBufLen = DEFAULT_BUFFER;

static int id = 1;

int main() {
    WSADATA wsaData;
    int iResult;

    // Инициализация Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed: %d\n", iResult);
        return 1;
    }

    struct addrinfo* result = NULL, hints;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Получение адресной информации
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    SOCKET listeningSock = INVALID_SOCKET;
    listeningSock = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (listeningSock == INVALID_SOCKET) {
        printf("Error at socket(): %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind(listeningSock, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(listeningSock);
        WSACleanup();
        return 1;
    }

    if (listen(listeningSock, SOMAXCONN) == SOCKET_ERROR) {
        printf("Listen failed with error: %ld\n", WSAGetLastError());
        closesocket(listeningSock);
        WSACleanup();
        return 1;
    }

    UserData users[2];
    int count{ 0 };

    while (count != 2) {
        SOCKET clientSock = INVALID_SOCKET;
        clientSock = accept(listeningSock, NULL, NULL);
        if (clientSock == INVALID_SOCKET) {
            printf("accept failed: %d\n", WSAGetLastError());
            closesocket(listeningSock);
            WSACleanup();
            return 1;
        }

        std::string userName = "user" + std::to_string(id);

        users[count] = UserData(userName, clientSock);
        count++;
        id++;
    }

    closesocket(listeningSock);

    while (true) {
        for (int i = 0; i < 2; i++) {
            recResult = recv(users[i].getSocket(), recBuf, DEFAULT_BUFFER, 0);
            if (recResult > 0) {
                printf("%s: %.*s\n", users[i].getName().c_str(), recResult, recBuf);
    
                std::string response = users[i].getName() + ":" + std::string(recBuf, recResult);
                
                for (int j = 0; j < 2; j++) {
                    send(users[j].getSocket(), response.c_str(), response.length(), 0);
                }
            }
            else if (recResult == 0) {
                printf("Connection closed by %s\n", users[i].getName().c_str());
            }
            else {
                printf("recv failed from %s: %d\n", users[i].getName().c_str(), WSAGetLastError());
            }
        }
    }

    for (int i = 0; i < 2; i++) {
        closesocket(users[i].getSocket());
    }

    WSACleanup();
    return 0;
}