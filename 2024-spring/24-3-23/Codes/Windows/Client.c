#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <direct.h>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "192.168.147.128"
#define PORT 10086
#define BUFFER_SIZE 1024

/** 将指定的SOCKET连接到服务器上 **/
int ConnectToServer(SOCKET* socket, SOCKADDR_IN* server_socket);
/** 向默认的socket发送单条信息 **/
int SendPieceMessage(const char* msg);
/** 向默认的socket发送一个文件 **/
int SendFile(const char* filename);
/** 从默认socket接收一条信息 **/
int ReceiveMessage(SOCKET clientSocket, char* buffer);
/** utils - 获取当前工作目录 **/
void getWorkingDirectory();

SOCKET clientSocket;
SOCKADDR_IN server_socket;
///*
int main()
{
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
    {
        printf("Failed to initialize Winsock.\n");
        return 1;
    }

    SendPieceMessage("FileOff");
    SendPieceMessage("Hi");

    SendFile("../test.txt");

    SendPieceMessage("quit");

    closesocket(clientSocket);
    WSACleanup();
    return 0;
}
//*/
int ConnectToServer(SOCKET* clientSocket, SOCKADDR_IN* server_socket)
{
    (*clientSocket) = socket(AF_INET, SOCK_STREAM, 0);
    if ((*clientSocket) == INVALID_SOCKET)
    {
        printf("Socket creation failed.\n");
        return 1;
    }

    (*server_socket).sin_addr.s_addr = inet_addr(SERVER_IP);
    (*server_socket).sin_port = htons(PORT);
    (*server_socket).sin_family = AF_INET;

    if (connect((*clientSocket), (SOCKADDR*)server_socket, sizeof(SOCKADDR)) == SOCKET_ERROR)
    {
        printf("Failed to connect to server. Error code: %d\n", WSAGetLastError());
        return 1;
    }
    return 0;
}

int SendPieceMessage(const char* msg)
{
    if (ConnectToServer(&clientSocket, &server_socket) != 0)
    {
        printf("Failed to connect to server.\n");
        return 1;
    }

    if (send(clientSocket, msg, strlen(msg), 0) == SOCKET_ERROR)
    {
        printf("Send failed with error code: %d\n", WSAGetLastError());
        return 1;
    }
    else
    {
        printf("Message: {  %s  } Sent!\n", msg);
    }

    char buffer[BUFFER_SIZE];
    ReceiveMessage(clientSocket, buffer);

    return 0;
}

int SendFile(const char* filename)
{
    // Start Server FileIn
    SendPieceMessage("FileIn");

    // File Open
    FILE* file = fopen(filename, "rb");
    if (file == NULL)
    {
        printf("Failed to open file: %s\n", filename);
        return 1;
    }

    // get Size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    SendPieceMessage("Test.txt");

    char buffer[BUFFER_SIZE];
    size_t read_size;

    // Read and send file in chunks
    while ((read_size = fread(buffer, 1, BUFFER_SIZE, file)) > 0)
    {
        if (send(clientSocket, buffer, read_size, 0) == SOCKET_ERROR)
        {
            printf("Send failed with error code: %d\n", WSAGetLastError());
            printf("Failed to send file chunk.\n");
            fclose(file);
            return 1;
        }
        else
        {
            printf("Message: {  %s  } Sent!\n", buffer);
        }
    }

    // Signal end of file
    send(clientSocket, "FileOff", 7, 0);
    memset(buffer, 0, BUFFER_SIZE);
    ReceiveMessage(clientSocket, buffer);

    // Close file
    fclose(file);

    return 0;
}

int ReceiveMessage(SOCKET clientSocket, char* buffer)
{
    int bytesReceived = recv(clientSocket, buffer, BUFFER_SIZE, 0);
    if (bytesReceived == SOCKET_ERROR)
    {
        printf("Failed to receive message. Error code: %d\n", WSAGetLastError());
        return 1;
    }

    buffer[bytesReceived] = '\0';
    printf("Received message from server: %s\n", buffer);

    return 0;
}

void getWorkingDirectory()
{
    char path[1024];

    if (_getcwd(path, sizeof(path)) != NULL)
    {
        printf("Current working directory: %s\n", path);
    }
    else
    {
        perror("getcwd() error");
    }
}
