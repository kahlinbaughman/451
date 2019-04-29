#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

void *connection_handler(void *);

int main (int argc, char *argv[])
{

    struct sockaddr_in serverAddr, clientAddr;
    int clientSocket, serverSocket;
    int optionValue, portNum;
    socklen_t clientLength;

    printf("%s", "Starting server.\n");

    printf("%s", "Getting the port number for the server to listen to.\n");
    //Gets the port number from the command line argument
    portNum = atoi(argv[1]);




    //Sets the the socket length of clientAddr
    clientLength = sizeof(clientAddr);

    //Assigns a socket with internet address, socket type,and protocol
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    //Set the socket options, may not be necessary

    //Forms the structure to be hold specifying the internet address, any address allowed and port num
    //setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &optionValue, sizeof(optionValue));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(portNum);

    printf("%s", "Binding server socket.\n");
    //Binds the socket to the connection
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    printf("%s\n", "Waiting for client to connect...");

    pthread_t thread_id;

    //Listens for a conenction
    listen(serverSocket, 5);

    while((clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLength)))
    {

     pthread_create(&thread_id , NULL ,  connection_handler , (void*) &clientSocket);
     pthread_join(thread_id , NULL);
     printf("%s\n", "Closing connection to client.");
     close(clientSocket);
    }
}

void *connection_handler(void *socket_desc)
{
    printf("%s\n", "Client connected.");

    int socket = *(int*)socket_desc;
    int size = 19120;
    int file;


    printf("%s\n", "Sending file to client.");

    file = open("spotify-songs.txt", O_RDONLY);


    sendfile(socket, file, NULL, size);
    close(file);
}
