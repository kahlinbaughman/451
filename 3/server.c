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


    // The structures for storing internet information such as portnumbers
    struct sockaddr_in serverAddr, clientAddr;
    // Variables to store sockets, port numbers, and sequence number
    int clientSocket, serverSocket, sequenceNum, portNum, sourcePort, serverLength;
    // Stores the length of the client socket
    socklen_t clientLength;
    // The array to store the header
    unsigned char threeWay[20];

    printf("%s", "Starting server.\n");


    //Gets the port number from the command line argument
    portNum = atoi(argv[1]);

    //Sets the the socket length of clientAddr
    clientLength = sizeof(clientAddr);
    serverLength = sizeof(serverAddr);

    //Assigns a socket with internet address, socket type,and protocol
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);


    //Set the socket options, may not be necessary

    //Forms the structure to be hold specifying the internet address, any address allowed and port num
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(portNum);


    //Binds the socket to the connection
    bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr));

    printf("%s\n", "Waiting for client...");

    //Listens for a conenction
    listen(serverSocket, 5);

    // Accepts the connection
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLength);

    printf("%s\n", "Client connected.");
    printf("%s\n", "Receiving SYN.");

    // Receives the header
    recv(clientSocket, threeWay, 20, 0);

    printf("%s", "The header looks like: ");
    for(int i = 0; i < 20; i++)
    {
        printf("%x", threeWay[i]);
    }

    printf("\n");

    // Gets the source port number
    getsockname(sourcePort, (struct sockaddr *) &serverAddr, &serverLength);
    sourcePort = ntohs(serverAddr.sin_port);


    printf("%s\n", "Setting ACK, ACK number, and changing portnumbers.");

    threeWay[0] = (sourcePort >> 8) & 0xFF;
    threeWay[1] = sourcePort & 0xFF;
    threeWay[2] = (portNum >> 8) & 0xFF;
    threeWay[3] = portNum & 0xFF;

    threeWay[8] = threeWay[4];
    threeWay[9] = threeWay[5];
    threeWay[10] = threeWay[6];
    threeWay[11] = threeWay[7] + 0x01;
    threeWay[13] = 0x12;

    srand(rand()*5);
    sequenceNum = rand();

    printf("%s\n", "Setting new sequence number.");

    threeWay[4] = (sequenceNum >> 24) & 0xFF;
    threeWay[5] = (sequenceNum >> 16) & 0xFF;
    threeWay[6] = (sequenceNum >> 8) & 0xFF;
    threeWay[7] = sequenceNum & 0xFF;

    printf("%s\n", "Sending response header (SYN-ACK).");

    // Sending the ACK header
    send(clientSocket, threeWay, 20, 0);

    printf("%s\n", "Receiving last response header.");

    recv(clientSocket, threeWay, 20, 0);

    printf("%s", "The header looks like: ");
    for(int i = 0; i < 20; i++)
    {
        printf("%x", threeWay[i]);
    }

    printf("\n");

    printf("%s\n", "Closing connection.");
    close(clientSocket);
    close(serverSocket);

}
