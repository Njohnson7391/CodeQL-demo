/*
/*Filename: Main.cpp
Project:SENG2040 – ASSIGNMENT 3
By: Neethu Johnson-8847391, Figil K Manoj-8845019
Date: 23 February 2024
Description: A program to develop a test client 
*/


#include <iostream>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <vector>


//Linking with the Ws2_32.lib library to use Winsock functions
#pragma comment (lib, "Ws2_32.lib")

void send_messages(const std::string& server, const std::string& port, const std::string& message)
{
    int iResult;

    //socket for client
    SOCKET socket_connect = INVALID_SOCKET;

    //for winsock data
    WSADATA wsadata;
    struct addrinfo* outcome = nullptr, * ptr = nullptr, hints;

    //initalize winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsadata);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed: " << iResult << std::endl;
        return;
    }

    //Decrease the value of the hints structure, which will indicate how 
    //the socket address structures will be chosen.
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

   //for resolving the server address and port
    iResult = getaddrinfo(server.c_str(), port.c_str(), &hints, &outcome);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << std::endl;
        WSACleanup();
        return;
    }

    //trying to connect to server
    for (ptr = outcome; ptr != nullptr; ptr = ptr->ai_next) {
        socket_connect = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        if (socket_connect == INVALID_SOCKET) {
            std::cerr << "Error at socket(): " << WSAGetLastError() << std::endl;
            WSACleanup();
            return;
        }

        //trying to connect to server
        iResult = connect(socket_connect, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(socket_connect);
            socket_connect = INVALID_SOCKET;
            continue;
        }
        //if connection established
        break;

    }

    //freeing the memory allocated by getaddrinfo
    freeaddrinfo(outcome);

    //If the server was unavailable, clean and then exit.
    if (socket_connect == INVALID_SOCKET) {
        std::cerr << "Unable to connect to server!" << std::endl;
        WSACleanup();
        return;
    }

    // for sending log messages to server
    iResult = send(socket_connect, message.c_str(), (int)strlen(message.c_str()), 0);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << std::endl;
        closesocket(socket_connect);
        WSACleanup();
        return;
    }


    //showing the output, the number of bytes sent
    std::cout << "Bytes Sent: " << iResult << std::endl;

    // close the socket and cleanup winsock
    closesocket(socket_connect);
    WSACleanup();

}


//main function
int main(int argc, char* argv[]) 
{
    //verifying the number of command line arguments
    if (argc < 4) {
        std::cerr << "Expected Usage: " << argv[0] << " <server> <port> <message>" << std::endl;
        return 1;
    }

    //Obtain the message, port, and server address from the command-line inputs.
    std::string server = argv[1];
    std::string port = argv[2];
    std::string message = argv[3];

    //for sending log messages
    send_messages(server, port, message);

    return 0;

}


/*
REFERENCES:
https://learn.microsoft.com/en-us/windows/win32/winsock/getting-started-with-winsock
https://stackoverflow.com/questions/4199185/socket-api-or-library-for-c
https://stackoverflow.com/questions/662328/what-is-a-simple-c-or-c-tcp-server-and-client-example
*/