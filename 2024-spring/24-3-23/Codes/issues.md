# Socket in C
## Codes in windows (Client)
### Finished Interfaces
```c
#define SERVER_IP "127.0.0.1"
#define PORT 10086
#define BUFFER_SIZE 1024

/* two global varibles to store server's config */
SOCKET clientSocket;
SOCKADDR_IN server_socket;

/** connect the given SOCKET with the server **/
int ConnectToServer(SOCKET* socket, SOCKADDR_IN* server_socket);
/** send a piece of msg to the server **/
int SendPieceMessage(const char* msg);
/** send a file to the server, if you want to change the filename, Change Code! **/
int SendFile(const char* filename);
/** recieve a msg from the server **/
int ReceiveMessage(SOCKET clientSocket, char* buffer);
/** utils - get current working directory, can be used to ensure the para in func "sendFile" is feasible **/
void getWorkingDirectory();
```

### Existing Problems
1. According to the principles of high cohesion and low coupling, sending a single message should be decoupled from receiving the message. However, in order to adapt to the current single-threaded server situation, coupling is necessary for the convenience of development.
2. Currently, sending files requires modifying the code to send different files, and specifying in the code where the server should receive the files. This should be changed to be determined by user input.

### Unfinsihed Solutions
1. Use a single thread to handle the connection, and use a queue to store the messages to be sent.
2. Seperate the sending and receiving of messages.
3. Add the ability to get user's input, then which file to be sent can be decided by user.

## Codes in linux (Server)
### Finished Interfaces
```c
#define PORT 10086
#define BUFFER_SIZE 1024
#define FILE_LENGTH_SIZE 65536

/** get file from a client **/
int GetFile();
```

### Existing Problems
1. The current implementation of the server is not suitable for a large number of clients. It can only handle one client at a time.
2. Current server use global variables to store the socket from client. This is not a good practice.

### Unfinished Solutions
1. Use a thread pool to handle multiple clients, when a client is accepted, create a new thread.
2. Make the socket a private variable of the thread.
3. Add more functions, such as sending files, create database, and so on.