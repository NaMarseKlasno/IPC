#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

void dispatch_2(int n);
void wSleep(void);

FILE *f;
int input;


int main(int argc, char* argv[])
{
    input = atoi(argv[1]);

    if ((f = fopen("p2.txt","r")) == NULL) {
        fprintf(stdout, "p2.txt was not open");
        exit(1);
    }

    kill(getppid(), SIGUSR1);

    (signal(SIGUSR1, dispatch_2)==SIG_ERR) ? exit(1) : wSleep();
}

void dispatch_2(int n)
{
    char arr[151];

    fscanf(f, "%s", arr); arr[strlen(arr)] = '\n';

    write(input, arr,strlen(arr) + 1);

    printf("p2: %s", arr);
}

void wSleep(void) {
    while(true)
        sleep(10);
}