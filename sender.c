// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <video_file>\n", argv[0]);

        exit(EXIT_FAILURE);
    }

    int sock;
    struct sockaddr_in server_addr;
    FILE *file;
    char buffer[BUFFER_SIZE];
    size_t bytes_read;

    // Open the video file
    file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("File open error");
        exit(EXIT_FAILURE);
    }

    // Create socket
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation error");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    // Set up server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Localhost
    server_addr.sin_port = htons(PORT);

    // Connect to the receiver
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection error");
        fclose(file);
        close(sock);
        exit(EXIT_FAILURE);
    }

    // Read and send the video file in segments
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        send(sock, buffer, bytes_read, 0);
    }

    // Send EOF indicator
    int eof_signal = -1;
    send(sock, &eof_signal, sizeof(eof_signal), 0);

    // Close the file and socket
    fclose(file);
    close(sock);

    return 0;
}

