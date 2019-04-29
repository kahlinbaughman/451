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
#include <pthread.h>

void *threadFunction(void *);

int main(int argc, char *argv[])
{
    int portNum;
    pthread_t thread_id[1];

    printf("%s", "Starting client.\n");

    portNum = atoi(argv[1]);

    printf("%s", "Attempting to connect to server...\n");

    pthread_create(&thread_id[0] , NULL ,  threadFunction , (void*) &portNum);
    pthread_join(thread_id[0], NULL);

    printf("%s", "File revieved. Closing connection.\n");

}

void *threadFunction(void *arg)
{

    struct sockaddr_in serverAddr;
    socklen_t serverLength;
    int serverSocket;
    char fileBuff[19120];
    int port;
    FILE *fp;

    port = *(int*)arg;

    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverLength = sizeof(serverAddr);

    connect(serverSocket, (struct sockaddr *)&serverAddr, serverLength);

    printf("%s", "Connected to server.\n");
    printf("%s", "Receiving file.\n");

    recv(serverSocket, fileBuff, sizeof(fileBuff), 0);
    fp = fopen("client-songs.txt", "w");
    fwrite(fileBuff, 1, sizeof(fileBuff), fp);
    fclose(fp);

}
