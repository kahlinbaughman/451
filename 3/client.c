#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/sendfile.h>
#include <arpa/inet.h>
#include <string.h>

int main(int argc, char *argv[])
{

    // Structures for storing socket related things such as portnumbers, and internet types
    struct sockaddr_in serverAddr, clientAddr;
    // Stores socket length
    socklen_t serverLength;
    // Various variables to to store int types
    int socketfd, destinationPort, sourcePort, clientLength, sequenceNum;
    // Header
    unsigned char threeWay[20];
    // Used for changing parts of array
    unsigned char temp;

    printf("%s", "Starting client.\n");

    // Gets port number to access
    destinationPort = atoi(argv[1]);
    srand(rand());
    sequenceNum = rand();

    serverLength = sizeof(serverAddr);
    clientLength = sizeof(clientAddr);

    // Sets sockets
    socketfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    // Sets type of connetion
    serverAddr.sin_family = AF_INET;
    // Sets port number for server
    serverAddr.sin_port = htons(destinationPort);

    printf("%s", "Attempting to connect to server...\n");

    // Connects to the server through the socket and port number
    connect(socketfd, (struct sockaddr *)&serverAddr, serverLength);

    // Gets origin port number
    getsockname(socketfd, (struct sockaddr *) &clientAddr, &clientLength);
    sourcePort = ntohs(clientAddr.sin_port);

    threeWay[0] = (sourcePort >> 8) & 0xFF;
    threeWay[1] = sourcePort & 0xFF;
    threeWay[2] = (destinationPort >> 8) & 0xFF;
    threeWay[3] = destinationPort & 0xFF;
    threeWay[4] = (sequenceNum >> 24) & 0xFF;
    threeWay[5] = (sequenceNum >> 16) & 0xFF;
    threeWay[6] = (sequenceNum >> 8) & 0xFF;
    threeWay[7] = sequenceNum & 0xFF;
    threeWay[8] = 0x00;
    threeWay[9] = 0x00;
    threeWay[10] = 0x00;
    threeWay[11] = 0x00;
    threeWay[12] = 0x00;
    threeWay[13] = 0x02;
    threeWay[14] = 0x44;
    threeWay[15] = 0x70;
    threeWay[16] = 0xFF;
    threeWay[17] = 0xFF;
    threeWay[18] = 0x00;
    threeWay[19] = 0x00;

    printf("%s\n", "Attempting threeWay handshake. Sending SYN.");
    printf("%s", "Header looks like: ");
    for(int i = 0; i < 20; i++)
    {
        printf("%x", threeWay[i]);
    }

    printf("\n");
    send(socketfd, threeWay, 20, 0);

    printf("%s\n", "Receiving response header.");

    recv(socketfd, threeWay, 20, 0);

    printf("%s", "Header looks like: ");
    for(int i = 0; i < 20; i++)
    {
        printf("%x", threeWay[i]);
    }

    printf("\n");

    printf("%s\n", "Changin ports, SYN, ACK, and SYN/ACK numbers");

    threeWay[0] = (sourcePort >> 8) & 0xFF;
    threeWay[1] = sourcePort & 0xFF;
    threeWay[2] = (destinationPort >> 8) & 0xFF;
    threeWay[3] = destinationPort & 0xFF;

    temp = threeWay[4];
    threeWay[4] = threeWay[8];
    threeWay[8] = temp;

    temp = threeWay[5];
    threeWay[5] = threeWay[9];
    threeWay[9] = temp;

    temp = threeWay[6];
    threeWay[6] = threeWay[10];
    threeWay[10] = temp;

    temp = threeWay[7];
    threeWay[7] = threeWay[11] + 0x01;
    threeWay[11] = temp + 0x01;

    threeWay[13] = 0x10;

    printf("%s\n", "Sending ACK");

    printf("%s", "Header looks like: ");
    for(int i = 0; i < 20; i++)
    {
        printf("%x", threeWay[i]);
    }

    printf("\n");

    send(socketfd, threeWay, 20, 0);



    printf("%s", "Connected to server.\n");


    printf("%s","Closing connection.\n");

}
