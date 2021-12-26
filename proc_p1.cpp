#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <errno.h>

void dispatch_1(int n);
void wSleep(void);

FILE *f;
int input;


int main(int argc, char* argv[])
{
    input = atoi(argv[1]);

    if ((f = fopen("p1.txt","r")) == NULL) {
        fprintf(stdout, "p1.txt was not open");
        exit(1);
    }

    kill(getppid(), SIGUSR1);

    (signal(SIGUSR1, dispatch_1)==SIG_ERR) ? (fclose(f), exit(EXIT_FAILURE)) : (wSleep());

    fclose(f);
    exit(EXIT_SUCCESS);
}

void dispatch_1(int n)
{
    size_t string_len = 300;
    char *arr = (char*)calloc(string_len, sizeof(char));

    if (getline(&arr, &string_len, f) == -1) {
        perror("read failed");
        exit(EXIT_FAILURE);
    }
    if (write(input, arr, strlen(arr)) == -1) {
        perror("write failed");
        exit(EXIT_FAILURE);
    } printf("P1 : %s", arr);

    free(arr);

//    if (arr == NULL) {
//        perror("calloc failed");
//        exit(EXIT_FAILURE);
//    } fscanf(f, "%s", arr); arr[strlen(arr)] = '\n';
//
//    if (write(input, arr, strlen(arr) + 1) == -1) {
//        perror("write failed");
//        exit(EXIT_FAILURE);
//    }
}

void wSleep(void) {
    while(true)
        sleep(10);
}