#include <iostream>
#include <thread>
#include <string>
#include <cstring>
#include <arpa/inet.h>
#include <unistd.h>
#include <sys/socket.h>

using namespace std;

void receiveMessages(int clientSocket) {
    char buffer[1024];

    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesReceived = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

        if (bytesReceived <= 0) {
            cout << "\nDisconnected from server.\n";
            break;
        }

        cout << buffer;
    }
}

int main() {
    int clientSocket;
    sockaddr_in serverAddress;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        cerr << "Error creating socket.\n";
        return 1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons(54000);
    inet_pton(AF_INET, "127.0.0.1", &serverAddress.sin_addr);

    if (connect(clientSocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) < 0) {
        cerr << "Connection failed.\n";
        close(clientSocket);
        return 1;
    }

    cout << "Connected to chat server.\n";
    cout << "Type your message and press Enter.\n";
    cout << "Type /quit to exit.\n";

    thread receiver(receiveMessages, clientSocket);

    string message;
    while (true) {
        getline(cin, message);

        if (message == "/quit") {
            break;
        }

        message += "\n";
        send(clientSocket, message.c_str(), message.size(), 0);
    }

    close(clientSocket);
    receiver.join();

    return 0;
}
