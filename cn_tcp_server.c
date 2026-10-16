
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>

#pragma comment(lib, "ws2_32.lib")

int main() {
    WSADATA wsa;
    SOCKET server_fd, client_fd;
    struct sockaddr_in server_addr, client_addr;
    int client_len = sizeof(client_addr);
    char buffer[1024];

    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        printf("WSAStartup failed\n");
        return 1;
    }

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd == INVALID_SOCKET) {
        printf("Socket creation failed\n");
        WSACleanup();
        return 1;
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(8080);

    if (bind(server_fd, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) == SOCKET_ERROR) {
        printf("Bind failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, 5) == SOCKET_ERROR) {
        printf("Listen failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Server waiting for client...\n");

    client_fd = accept(server_fd, (struct sockaddr *)&client_addr,
                       &client_len);

    if (client_fd == INVALID_SOCKET) {
        printf("Accept failed\n");
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    printf("Client connected.\n");

    while (1) {
        memset(buffer, 0, sizeof(buffer));

        int n = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

        if (n <= 0)
            break;

        buffer[n] = '\0';
        printf("Client: %s\n", buffer);

        if (strcmp(buffer, "exit") == 0)
            break;

        printf("Enter reply: ");

        if (fgets(buffer, sizeof(buffer), stdin) == NULL)
            break;

        buffer[strcspn(buffer, "\n")] = '\0';

        if (send(client_fd, buffer, (int)strlen(buffer), 0)
            == SOCKET_ERROR)
            break;

        if (strcmp(buffer, "exit") == 0)
            break;
    }

    closesocket(client_fd);
    closesocket(server_fd);
    WSACleanup();

    printf("Connection closed.\n");

    return 0;
}
