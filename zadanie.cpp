#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/wait.h>


int Serv1, Serv2, PR, P1, P2, T, S, D;

struct {
    int pipefd_1[2];
    int pipefd_2[2];
} pipes;

// ***** buffer *****
struct {
    int SEM1, SEM2; // ***** semaphores
    int SHM1, SHM2; // ***** shared memory

    char for_pipe_3[3];
    char PIPE10[20];
    char PIPE20[20];
    char PIPE21[10];

    char P1_char[10];
    char P2_char[10];
    char S1_char[10];
    char S2_char[10];
    char SHM1_char[10];
    char SHM2_char[10];
} buff;

void dispatch_1(int n);
void dispatch_2(int n);


int main(int argc, char* argv[])
{
    // ***** check ports
    if (argc != 3) {
        perror("Invalid ports");
        exit(EXIT_FAILURE);
    }

    signal(SIGUSR2, dispatch_2);
    signal(SIGUSR1, dispatch_1);

    // ***** display inputs
    fprintf(stdout, "Entered ports: %s\t%s\n", argv[1], argv[2]);

    // ***** create pipes
    if (pipe(pipes.pipefd_1)==-1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    } if (pipe(pipes.pipefd_2)==-1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    ////////////////////////////////////////////////////////////////////////////

    // ***** create proc_p1 with argv
    if ((P1 = fork()) == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } if (P1 == 0) {
        sprintf(buff.for_pipe_3, "%d", pipes.pipefd_1[1]);
        char* arr_P1[] = {(char*)"proc_p1", buff.for_pipe_3, NULL};
        execve("proc_p1", arr_P1, NULL);
    } sleep(1);


    // ***** create proc_p2 with argv
    if ((P2 = fork()) == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } if (P2 == 0) {
        char* arr_P2[] = {(char*)"proc_p2", buff.for_pipe_3, NULL};
        execve("proc_p2", arr_P2, NULL);
    } sleep(1);

    ////////////////////////////////////////////////////////////////////////////

    // ***** atoi convert
    sprintf(buff.PIPE21, "%d", pipes.pipefd_2[1]);
    sprintf(buff.PIPE20, "%d", pipes.pipefd_2[0]);
    sprintf(buff.PIPE10, "%d", pipes.pipefd_1[0]);
    sprintf(buff.P1_char, "%d", P1);
    sprintf(buff.P2_char, "%d", P2);

    // ***** argv for proc_pr
    char* arr_PR[] = { (char*)"proc_pr", buff.P1_char, buff.P2_char, buff.PIPE10, buff.PIPE21, NULL };

    // ***** create proc_pr
    if ((PR = fork()) == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } if (PR == 0) execve("proc_pr", arr_PR,NULL);
    sleep(1);

    ////////////////////////////////////////////////////////////////////////////

    // ***** create proc serv1 with argv
    if ((Serv1 = fork()) == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (Serv1 == 0) {
        char* arr_SERV1[] = {(char *)"proc_serv1", argv[1], argv[2], NULL};
        execve("proc_serv1", arr_SERV1,NULL);
    } sleep(1);

    // ***** create proc serv2 with argv
    if ((Serv2 = fork()) == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (Serv2 == 0) {
        char* arr_SERV2[] = {(char*)"proc_serv2", argv[2], NULL};
        execve("proc_serv2", arr_SERV2,NULL);
    }

    ////////////////////////////////////////////////////////////////////////////

    // ***** generate keys
    key_t key1 = ftok("p1.txt", 1);

    // ***** create semaphore (SEM1)
    if ((buff.SEM1 = semget(key1, 2, IPC_CREAT|0666)) < 0) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    // ***** generate keys
    key_t key2 = ftok("p2.txt", 1);

    // ***** create semaphore (SEM2)
    if ((buff.SEM2 = semget(key2, 2, IPC_CREAT|0666)) < 0) {
        perror("semget failed");
        exit(EXIT_FAILURE);
    }

    // ***** atoi convert
    sprintf(buff.S1_char, "%d", buff.SEM1);
    sprintf(buff.S2_char, "%d", buff.SEM2);

    ////////////////////////////////////////////////////////////////////////////

    // ***** create shared memory
    if ((buff.SHM1 = shmget(key1, 4096, IPC_CREAT|0666)) < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    } if ((buff.SHM2 = shmget(key2, 4096, IPC_CREAT|0666)) < 0) {
        perror("shmget failed");
        exit(EXIT_FAILURE);
    } fprintf(stdout, "%d\n%d\n", buff.SHM1, buff.SHM2);

    // ***** atoi convert
    sprintf(buff.SHM1_char, "%d", buff.SHM1);
    sprintf(buff.SHM2_char, "%d", buff.SHM2);

    ////////////////////////////////////////////////////////////////////////////

    // ***** create proc_t
    if ((T = fork()) == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (T == 0) {
        char* arr_PROC_T[] = {(char *)"proc_t", buff.S1_char, buff.SHM1_char, buff.PIPE20, NULL};
        execve("proc_t", arr_PROC_T,NULL);
    }

    ////////////////////////////////////////////////////////////////////////////

    // ***** create proc_s
    if ((S = fork()) == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (S == 0) {
        char *arr_PROC_S[] = {(char *)"proc_s", buff.SHM1_char, buff.S1_char, buff.SHM2_char, buff.S2_char, NULL};
        execve("proc_s", arr_PROC_S,NULL);
    }

    ////////////////////////////////////////////////////////////////////////////

    // ***** create proc_d
    if ((D = fork()) == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else if (D == 0) {
        char *arr_PROC_D[] = {(char *)"proc_d", buff.S2_char, buff.SHM2_char, argv[1], NULL};
        execve("proc_d", arr_PROC_D,NULL);
    }

    ////////////////////////////////////////////////////////////////////////////

    /// TODO: dadelat escho eto
    while (wait(NULL) > 0);

    if (semctl(buff.SEM1, 2, IPC_RMID)    == -1) perror("semctl failed");
    if (semctl(buff.SEM2, 2, IPC_RMID)    == -1) perror("semctl failed");
    if (shmctl(buff.SHM1, IPC_RMID, NULL) == -1) perror("shmctl failed");
    if (shmctl(buff.SHM2, IPC_RMID, NULL) == -1) perror("shmctl failed");

    ////////////////////////////////////////////////////////////////////////////

    exit(EXIT_SUCCESS);
}

void dispatch_1(int n) {}

void dispatch_2(int n)
{
    sleep(3);
    kill(P1, SIGKILL);
    kill(P2, SIGKILL);
    kill(PR, SIGKILL);
    kill(T, SIGKILL);
    kill(S, SIGKILL);
    kill(D, SIGKILL);
    kill(Serv1, SIGKILL);
    kill(Serv2, SIGKILL);
}