#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib") // Link with winsock library

#define PORT 8000

int main()
{
    WSADATA wsa;
    SOCKET server_socket, clinet_socket;
    struct sockaddr_in server, client;
    int client_len;
    char buffer[1024];
    // INITIALIZING WINKSOCK

    printf("Initializing winsock....\n");
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("WSASTARTUP failed.Error code: %d\n", WSAGetLastError());
        return 1;
    }

    // create a socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET)
    {
        printf("Could not connect to a socket: %d\n", WSAGetLastError());
        return 1;
    }

    // Prepare sockaddr in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // BIND
    if (bind(server_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
    {
        printf("Bind failed with error code: %d", WSAGetLastError()); // WSAGETLASTERROR function gets the error from the server
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    printf("Bind done.\n");

    // listen
    listen(server_socket, 3);
    printf("Waiting for incoming connections on PORT %d\n", PORT);

    // Accept a connection
    //Loop so i can handle multiple reqst 😎😎
    while (1)
    {
        client_len = sizeof(client);
        clinet_socket = accept(server_socket, (struct sockaddr *)&client, &client_len);
        if (clinet_socket == INVALID_SOCKET)
        {
            printf("Accept failed: %d\n", WSAGetLastError());
            continue;
        }

        printf("Connection accepted.\n");

        // Receive data
        memset(buffer, 0, sizeof(buffer));

        int bytes_received = recv(clinet_socket, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received == SOCKET_ERROR)
        {
            printf("recv() failed. Error: %d\n", WSAGetLastError());
            closesocket(clinet_socket);
            continue;
        }
        buffer[bytes_received] = '\0';
        printf("[*] Received:\n%s\n", buffer);

        // Send a message back to client
        char *http_response =
            "HTTP/1.1 200 OK\r\n"
            "Content-Type: text/html\r\n"
            "Connection: close\r\n"
            "\r\n"
            "<!DOCTYPE html>"
            "<html><head><title>My C Server</title></head>"
            "<body><h1>Hello from WinSock HTTP server!</h1></body></html>";
        
        int sent =send(clinet_socket, http_response, strlen(http_response), 0); 
        if (sent == SOCKET_ERROR)
        {
            printf("send failed: %d\n", WSAGetLastError());
        }
        

        // Close sockets and cleanup
        closesocket(clinet_socket);
        printf("Client connection closed.\n\n");


    }
    closesocket(server_socket);
    WSACleanup();

    return 0;
}