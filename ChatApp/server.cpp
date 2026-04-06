#include <iostream>
#include <thread>
#include <vector>
#include <string>
#include <mutex>
#include <algorithm>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

using namespace std;

vector<int> clients;
mutex clientsMutex;

void sendToAllClients(const string& message, int senderSocket) {
    lock_guard<mutex> lock(clientsMutex);
    for (int clientSocket : clients) {
        if (clientSocket != senderSocket) {
            send(clientSocket, message.c_str(), message.size(), 0);
        }
    }
}

void handleClient(int clientSocket, sockaddr_in clientAddress) {
    char buffer[1024];
    string clientIP = inet_ntoa(clientAddress.sin_addr);
    int clientPort = ntohs(clientAddress.sin_port);

    string joinMessage = "Client [" + clientIP + ":" + to_string(clientPort) + "] joined the chat.\n";
    cout << joinMessage;
    sendToAllClients(joinMessage, clientSocket);

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            string leaveMessage = "Client [" + clientIP + ":" + to_string(clientPort) + "] left the chat.\n";
            cout << leaveMessage;
            sendToAllClients(leaveMessage, clientSocket);

            lock_guard<mutex> lock(clientsMutex);
            clients.erase(remove(clients.begin(), clients.end(), clientSocket), clients.end());

            close(clientSocket);
            break;
        }

        string message = "Client [" + clientIP + ":" + to_string(clientPort) + "]: " + string(buffer);
        cout << message;
        sendToAllClients(message, clientSocket);
    }
}

int main() {
    int serverSocket, clientSocket;
    sockaddr_in serverAddress, clientAddress;
    socklen_t clientLength = sizeof(clientAddress);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        cerr << "Error creating socket.\n";
        return 1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(54000);

    if (bind(serverSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Bind failed.\n";
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 5) < 0) {
        cerr << "Listen failed.\n";
        close(serverSocket);
        return 1;
    }

    cout << "Server is running on port 54000...\n";

    while (true) {
        clientSocket = accept(serverSocket, (sockaddr*)&clientAddress, &clientLength);

        if (clientSocket < 0) {
            cerr << "Accept failed.\n";
            continue;
        }

        {
            lock_guard<mutex> lock(clientsMutex);
            clients.push_back(clientSocket);
        }
        thread clientThread(handleClient, clientSocket, clientAddress);
        clientThread.detach();
    }
    close(serverSocket);
    return 0;
}
