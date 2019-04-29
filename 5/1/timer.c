#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int timer = 0;
int global = 1;

void count(int signum) { //signal handler
    printf("Hello World!\n");
    timer++;
    global = 0;
    //exit(1); //exit after printing
}

void handler(int signum) { //signal handler
    printf("\n%d\n", timer);
    exit(1); //exit after printing
}

int main(int argc, char const *argv[]) {
    while (1)
    {
        signal(SIGINT, handler);
        signal(SIGALRM,count);
        alarm(1);
        while(global);
        global = 1;
        //printf("%s\n", "Testing");

    }
}
