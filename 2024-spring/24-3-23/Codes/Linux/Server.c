#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 10086
#define BUFFER_SIZE 1024
#define FILE_LENGTH_SIZE 65536

int GetFile();
int serverSocket, msgSocket;
struct sockaddr_in address;
int addrlen = sizeof(address);

int main() {
    char receiveBuffer[BUFFER_SIZE], sendBuffer[BUFFER_SIZE];
    struct sockaddr_in address;
    int serverSocket, msgSocket, addrlen = sizeof(address);

    // Initialize address structure
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    // Create socket
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        exit(EXIT_FAILURE);
    }

    // Bind address and port
    if (bind(serverSocket, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    // Listen for connections
    if (listen(serverSocket, 5) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }
    printf("Server >> ready to get connection\n");

    while(1) {
        memset(sendBuffer, 0, BUFFER_SIZE);
        memset(receiveBuffer, 0, BUFFER_SIZE);
        msgSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (msgSocket < 0) {
            perror("Accept failed");
            continue;  // Continue to the next iteration of the loop
        }
        printf("Server >> Get Client\n");

        recv(msgSocket, receiveBuffer, BUFFER_SIZE, 0);

        if (strncmp(receiveBuffer, "quit", 4) == 0) {
            printf("Server is exiting...\n");
            strcat(sendBuffer, "Server >> Exiting...");
            send(msgSocket, sendBuffer, strlen(sendBuffer), 0);
            break;  // Exit the loop and close the server
        }
        else if (strncmp(receiveBuffer, "FileIn", 6) == 0) {
            printf("Find \"FileIn\" Command!\n");
            strcat(sendBuffer, "Server >> < File In Mode Start ! >\n");
            send(msgSocket, sendBuffer, strlen(sendBuffer), 0);
            if (GetFile() != 0) {
                printf("File receive failed\n");
            } // Start file operation
        }
        else {
            strcat(sendBuffer, "Server >> Message: ");
            strcat(sendBuffer, receiveBuffer);
            strcat(sendBuffer, " Got!");
            send(msgSocket, sendBuffer, strlen(sendBuffer), 0);
            printf("Reply sent to client: %s\n", sendBuffer);
        }
    }

    close(msgSocket);
    close(serverSocket);
    return 0;
}

int GetFile() {
    // Implement file receiving logic here
    printf("< File In Mode Start ! >\n");
    msgSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    char filename[BUFFER_SIZE] = {0};
    recv(msgSocket, filename, BUFFER_SIZE, 0);

    char sendBuffer[BUFFER_SIZE] = {0};
    strcat(sendBuffer, "Server >> FileName: ");
    strcat(sendBuffer, filename);
    strcat(sendBuffer, " Got!");
    send(msgSocket, sendBuffer, strlen(sendBuffer), 0);
    printf("Reply sent to client: %s\n", sendBuffer);
    memset(sendBuffer, 0, BUFFER_SIZE);

    int i;
    for (i = 0; i < BUFFER_SIZE; i++) {
        if (filename[i] == '\0' || filename[i] == '\n') {
            break;
        }
    }
    if (i < BUFFER_SIZE) {
        for (int j = i + 1; j < BUFFER_SIZE; j++) {
            filename[j] = '\0';
        }
    } else {
        memset(filename, 0, BUFFER_SIZE);
    }

    FILE *fptr;
    fptr = fopen(filename, "wb");
    if (fptr == NULL)
    {
        fprintf(stderr, "Error opening file for writing\n");
        return 1;
    }

    char receiveBuffer[FILE_LENGTH_SIZE] = {0};
    ssize_t bytesReceived;

    msgSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    bytesReceived = recv(msgSocket, receiveBuffer, FILE_LENGTH_SIZE, 0);
    if (bytesReceived > 0) {
        fwrite(receiveBuffer, 1, (size_t)bytesReceived, fptr);
    }

    strcat(sendBuffer, "Server >> File received Finish!");
    send(msgSocket, sendBuffer, strlen(sendBuffer), 0);
    fclose(fptr);
    return 0;
}