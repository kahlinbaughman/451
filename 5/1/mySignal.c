/* mySignal.c */
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>


int global = 1;

void handler(int signum) { //signal handler
    printf("Hello World!\n");
    global = 0;
    //exit(1); //exit after printing
}
int main(int argc, char * argv[]){
    while(1)
    {
    signal(SIGALRM,handler); //register handler to handle SIGALRM
    alarm(1); //Schedule a SIGALRM for 1 second
    while(global); //busy wait for signal to be delivered
    printf("Turing was right!\n");
    global = 1;
    // return 0; //never reached
    }
}
