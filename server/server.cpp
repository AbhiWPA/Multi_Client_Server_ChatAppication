#include <iostream>
#include <winsock2.h>
#include <vector>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define PORT 5000

void clientHandler(SOCKET clientSocket, std::vector<SOCKET>& clientSockets)
{
    char buffer[4096];
    std::string clientMessage;
    std::string serverMessage;

    while (true)
    {
        memset(buffer, 0, sizeof(buffer));

        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesReceived == SOCKET_ERROR)
        {
            std::cerr << "Failed to receive data from the client" << std::endl;
            break;
        }
        else if (bytesReceived == 0)
        {
            std::cout << "Client disconnected" << std::endl;
            break;
        }

       
        std::cout << "Client Says: " << clientMessage << std::endl;
        std::cout<<"Enter message for client : ";
        std::cin>>serverMessage;
        std::cout << "" <<std::endl;
        serverMessage = buffer;

        // Echo the message back to the client
        if (send(clientSocket, buffer, bytesReceived, 0) == SOCKET_ERROR)
        {
            std::cerr << "Failed to send data to the client" << std::endl;
            break;
        }
    }

    // Remove the client socket from the vector
    for (size_t i = 0; i < clientSockets.size(); ++i)
    {
        if (clientSockets[i] == clientSocket)
        {
            clientSockets.erase(clientSockets.begin() + i);
            break;
        }
    }

    closesocket(clientSocket);
}

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        std::cerr << "Failed to initialize winsock" << std::endl;
        return 1;
    }

    SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == INVALID_SOCKET)
    {
        std::cerr << "Failed to create socket" << std::endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(PORT);
    serverAddress.sin_addr.s_addr = INADDR_ANY;

    if (bind(serverSocket, reinterpret_cast<sockaddr*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR)
    {
        std::cerr << "Failed to bind the socket" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    if (listen(serverSocket, SOMAXCONN) == SOCKET_ERROR)
    {
        std::cerr << "Failed to listen on the socket" << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::vector<SOCKET> clientSockets;

    std::cout << "Server has started on port: " << PORT << std::endl;

    while (true)
    {
        SOCKET clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Failed to accept client connection" << std::endl;
            break;
        }

        std::cout << "New client connected" << std::endl;

        // Add the client socket to the vector
        clientSockets.push_back(clientSocket);

        // Create a thread to handle the client
        std::thread clientThread(clientHandler, clientSocket, std::ref(clientSockets));
        clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
