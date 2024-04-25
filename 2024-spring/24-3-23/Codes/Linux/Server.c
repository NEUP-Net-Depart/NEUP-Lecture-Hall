#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 10086
#define BUFFER_SIZE 1024

int GetFile();
int serverSocket, msgSocket;
struct sockaddr_in address;
int addrlen = sizeof(address);

int main() {
    char receiveBuffer[BUFFER_SIZE], sendBuffer[BUFFER_SIZE];

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(PORT);

    // Create socket
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    // Bind address and port
    bind(serverSocket, (struct sockaddr *)&address, sizeof(address));

    // Listen for connections
    listen(serverSocket, 5);
    printf("Server >> ready to get connection\n");


    while(1) {
        memset(sendBuffer, 0, BUFFER_SIZE);
        memset(receiveBuffer, 0, BUFFER_SIZE);
        msgSocket = accept(serverSocket, (struct sockaddr *)&address, (socklen_t*)&addrlen);
        if (msgSocket) {
            printf("Server >> Get Client\n");
        }

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
        else if (msgSocket != -1) {
            strcat(sendBuffer, "Server >> Message: ");
            strcat(sendBuffer, receiveBuffer);
            strcat(sendBuffer, " Got!");
            send(msgSocket, sendBuffer, strlen(sendBuffer), 0);
            printf("Reply sent to client: %s\n", sendBuffer);
        } else {
            printf("Server >> Failed to receive message from client\n");
        }
    }

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
    for (i = 0; i < BUFFER_SIZE; ++i) {
        if (filename[i] == '\0' || filename[i] == '\n') {
            break;
        }
    }
    //printf("debug: %d\n",strlen(filename));
    if (i < BUFFER_SIZE) {
        for (int j = i + 1; j < BUFFER_SIZE; j++) {
            filename[j] = '\0';
        }
    } else {
        memset(filename, 0, BUFFER_SIZE);
    }

    FILE *fptr;
    fptr = fopen(filename, "wb");
    if (fptr == NULL) {
        fprintf(stderr, "Error opening file for writing\n");
        return 1;
    }

    char receiveBuffer[BUFFER_SIZE] = {0};
    ssize_t bytesReceived;
    printf("Server >> Ready to recieve now!\n");

    while (1) {
        bytesReceived = recv(msgSocket, receiveBuffer, BUFFER_SIZE, 0);
        if (bytesReceived < 0) {
            fprintf(stderr, "Error in receiving file data\n");
            fclose(fptr);
            return 1;
        } else if (bytesReceived == 0) {
            // 连接关闭或其他错误处理
            printf("Server >> while in file, break!");
            fclose(fptr);
            return 1;
        } else if (strncmp(receiveBuffer, "FileOff", 7) == 0) {
            printf("Server >> File received Finish!\n");
            memset(sendBuffer, 0, BUFFER_SIZE);
            strcat(sendBuffer, "Server >> File received Finish!");
            send(msgSocket, sendBuffer, strlen(sendBuffer), 0);
            fclose(fptr);
            return 0;
        } else {
            send(msgSocket, "Server >> File data received!", 29, 0);
            printf("Server >> File data received!\n");
            fwrite(receiveBuffer, 1, (size_t) bytesReceived, fptr);
            memset(receiveBuffer, 0, BUFFER_SIZE);
        }
    }

    fclose(fptr);
    return 0;
}