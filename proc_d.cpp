#include <stdio.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/wait.h>

void p_Semafor(int id_Semafor);
void w_Semafor(int id_Semafor);

void p_Semafor(int id_Semafor){
    struct sembuf semafaor_buffer_p;
    int error;
    semafaor_buffer_p.sem_op = 1;
    semafaor_buffer_p.sem_num = 0;
    semafaor_buffer_p.sem_flg = 0;
    error = semop(id_Semafor, &semafaor_buffer_p, 1);
    if(error == -1){
        exit(1);
    }
}

void w_Semafor(int id_Semafor){
    struct sembuf semafaor_buffer_w;
    int error;
    semafaor_buffer_w.sem_op = -1;
    semafaor_buffer_w.sem_num = 1;
    semafaor_buffer_w.sem_flg = 0;
    error = semop(id_Semafor, &semafaor_buffer_w, 1);
    if(error == -1){
        exit(1);
    }
}

int main(int argc , char *argv[]){
    char *shmat_2;
    char *Array;
    int Socket = 0;
    int v;
    long int size_Array;
    int error;
    struct sockaddr_in addr_Server;
    int Server_port1 = atoi(argv[3]);
    int VZ_check2 = atoi(argv[2]);
    int semafor_2 = atoi(argv[1]);

    shmat_2 = ( char *)shmat(VZ_check2,NULL,0);
    Array = (char*)calloc(151,sizeof(char));


    Socket = socket(AF_INET, SOCK_STREAM, 0);
    if (Socket < 0) {
        printf("\n Error creation socket \n");
        return -1;
    }

    bzero((char *) &addr_Server, sizeof(addr_Server));
    addr_Server.sin_family = AF_INET;
    addr_Server.sin_port = htons(Server_port1);

    error = inet_pton(AF_INET, "127.0.0.1", &addr_Server.sin_addr.s_addr);
    if(error <= 0){
        printf("\n Address not supported / Invalid address \n");
        return -1;
    }

    error = connect(Socket,(struct sockaddr *)&addr_Server, sizeof(addr_Server));
     if (error < 0){
        perror("\nConnection error \n");
        return -1;
    }
    puts("Successful connection\n");

    p_Semafor(semafor_2);
    v = 1;
    for(;v < 11; v++){
        w_Semafor(semafor_2);
        strcpy(Array,shmat_2);
        size_Array = strlen(Array);
        Array[size_Array] = '\0';
        printf("Got word %d %ld %s\n",v,size_Array,Array);
        send(Socket , Array , size_Array+1 , 0 );
        sleep(2);
        p_Semafor(semafor_2);
        if(10 <= v){
            kill(getppid(),SIGUSR2);
        }
    }
      return 0;
}


