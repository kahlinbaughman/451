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

    struct sockaddr_in serverAddr;
    int serverSocket;
    int file, portNum;
    char fileBuff[28121];
    socklen_t serverLength;
    FILE *fp;

    printf("%s", "Starting client.\n");

    portNum = atoi(argv[1]);

    serverLength = sizeof(serverAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(portNum);

    printf("%s", "Attempting to connect to server...\n");

    connect(serverSocket, (struct sockaddr *)&serverAddr, serverLength);

    printf("%s", "Connected to server.\n");
    printf("%s", "Receiving file.\n");

    recv(serverSocket, fileBuff, sizeof(fileBuff), 0);
    fp = fopen("berserk-wiki.html", "w+");
    fputs(fileBuff, fp);

    printf("%s", "File revieved. Closing connection.\n");

}
