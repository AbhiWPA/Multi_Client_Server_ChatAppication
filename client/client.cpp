#include <iostream>
#include <winsock2.h>
#include <string>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize winsock" << std::endl;
        return 1;
    }

    SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(clientSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to connect to the server" << std::endl;
        closesocket(clientSocket);
        WSACleanup();
        return 1;
    }

    char buffer[4096];
    std::string clientMessage;
    std::string serverMessage;

    while (true)
    {
        std::cout << "Enter Your Message: ";
        std::getline(std::cin, clientMessage);

        if (clientMessage == "exit")
        {
            std::cout<<"Client is disconnected";
            break;
        }

        if (send(clientSocket, clientMessage.c_str(), clientMessage.size(), 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to send data to the server" << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        //std::cerr << "server connected" << std::endl;
        memset(buffer, 0, sizeof(buffer));
        serverMessage = buffer;

        if (recv(clientSocket, buffer, sizeof(buffer), 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to receive data from the server" << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        serverMessage = buffer;
        std::cout << "Server Says: " << serverMessage << std::endl;
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}
