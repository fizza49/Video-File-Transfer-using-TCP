// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main() {
    int sock, new_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);
    FILE *file;
    char buffer[BUFFER_SIZE];
    size_t bytes_received;

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind error");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(sock, 1) < 0) {
        perror("Listen error");
        close(sock);
        exit(EXIT_FAILURE);
    }

    printf("Waiting for a connection...\n");

    // Accept a connection
    new_sock = accept(sock, (struct sockaddr *)&client_addr, &addr_len);
    if (new_sock < 0) {
        perror("Accept error");
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Open a new video file for writing
    file = fopen("received_video.mp4", "wb");
    if (file == NULL) {
        perror("File open error");
        close(new_sock);
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Receive the video file in segments
    while (1) {
        bytes_received = recv(new_sock, buffer, BUFFER_SIZE, 0);
        if (bytes_received <= 0) {
            break; // Break on EOF signal
        }
        fwrite(buffer, 1, bytes_received, file);
    }

    // Close the file and sockets
    fclose(file);
    close(new_sock);
    close(sock);

    printf("File received successfully!\n");

    return 0;
}

