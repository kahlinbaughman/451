#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <string.h>

int main (int argc, char *argv[])
{



    struct sockaddr_in serverAddr, clientAddr;
    int clientSocket, serverSocket;
    int file, optionValue, portNum;
    socklen_t clientLength;

    printf("%s", "Starting server.\n");

    // char message[32] = "Closing connection to server.\n";
    optionValue = 1;

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


    //Binds the socket to the connection
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    printf("%s\n", "Waiting for client...");

    //Listens for a conenction
    listen(serverSocket, 5);

        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLength);

        printf("%s\n", "Client connected.");



        file = open("berserk-wikipedia.html", O_RDONLY);

        printf("%s\n", "Sending file to client.");

        sendfile(clientSocket, file, NULL, 28120);


        close(file);


        printf("%s\n", "Closing connection.");
        close(clientSocket);
        close(serverSocket);

}
