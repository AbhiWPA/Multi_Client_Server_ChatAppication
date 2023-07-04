#include <iostream>
#include <winsock2.h>
#include <vector>
#include <mutex>
#include <thread>
#include <unistd.h>

#pragma comment(lib, "ws2_32.lib")

constexpr int PORT = 5000;

class ClientHandler {
    private:
        std::vector<ClientHandler*>& clientHandlersList;
        std::mutex& mutex;

    public:
        int clientSocket;
        ClientHandler(int socket, std::vector<ClientHandler*>& clients, std::mutex& lock):
         clientSocket(socket), clientHandlersList(clients), mutex(lock){

         }

         void operate(){
            try{
                int bytesRead;

            char buffer[4096];
            std::string clientMessage;
            std::string serverMessage;

            while (true){
                bytesRead = 0;

                memset(buffer, 0, sizeof(buffer));
                clientMessage = buffer;
                bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

                // cout << clientSocket << std::endl
                //      << buffer << std::endl
                //      << sizeof(buffer) << std::endl
                //      << bytesRead << std::endl;

                std::cout << "Client Says : " << buffer << std ::endl;

                // Sending again to the Client
                std:: cout << "Enter a message to Client : ";
                std::getline(std::cin, serverMessage);

                send(clientSocket, serverMessage.c_str(), serverMessage.length(), 0);
                // send(clientSocket, buffer, bytesRead, 0);
            }

            if (bytesRead == 0)
            {
                // Client disconnected
                std::cout << "Client disconnected." << std::endl;
            }
            else
            {
                // Error in receiving data
                std::cerr << "Error in receiving data from client." << std::endl;
            }

            close(clientSocket);

            // Remove the client from the list
            // std::lock_guard<std::mutex> lock(mtx);
            // clientHandlersList.erase(std::remove(clientHandlersList.begin(), clientHandlersList.end(), this),
            // clientHandlersList.end());

            }catch(const std::exception& e){
                std:: cout << e.what() << std::endl;
            }
         }
        

    char buffer[4096];
    std::string clientMessage;

    // while (true)
    // {
    //     memset(buffer, 0, sizeof(buffer));

    //     int bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
    //     if (bytesReceived == SOCKET_ERROR)
    //     {
    //         std::cerr << "Failed to receive data from the client" << std::endl;
    //         break;
    //     }
    //     else if (bytesReceived == 0)
    //     {
    //         std::cout << "Client disconnected" << std::endl;
    //         break;
    //     }

    //     clientMessage = buffer;
    //     std::cout << "Client Says: " << clientMessage << std::endl;

    //     // Echo the message back to the client
    //     if (send(clientSocket, buffer, bytesReceived, 0) == SOCKET_ERROR)
    //     {
    //         std::cerr << "Failed to send data to the client" << std::endl;
    //         break;
    //     }
    // }

    // // Remove the client socket from the vector
    // for (size_t i = 0; i < clientSockets.size(); ++i)
    // {
    //     if (clientSockets[i] == clientSocket)
    //     {
    //         clientSockets.erase(clientSockets.begin() + i);
    //         break;
    //     }
    // }

    // closesocket(clientSocket);
};

int main(){
    WSADATA wsaData;

    std::vector<ClientHandler *> clientHandlersList;
    std::mutex mtx;

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

        // Create a new ClientHandler instance
        ClientHandler *client = new ClientHandler(clientSocket, clientHandlersList, mtx);

        // Add the client for handling in a separate thread
        clientHandlersList.push_back(client);

        std::thread clientThread(
            [&client]()
            {
                client->operate();
            });

        clientThread.detach();


        // Create a thread to handle the client
        // std::thread clientThread(clientHandler, clientSocket, std::ref(clientSockets));
        // clientThread.detach();
    }

    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
