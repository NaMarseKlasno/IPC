#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <signal.h>
#include <sys/stat.h>
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h> 
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

void semSetup(int sem1_id,char option){
    struct sembuf sops;
    if(option=='w'){//wait
        sops.sem_num = 0;
        sops.sem_op = -1;
    }else{
        sops.sem_num = (option=='1') ? 1 /*sem1*/
                                     : 0;//sem2
        sops.sem_op = 1;
    }
    sops.sem_flg = 0;
    semop(sem1_id, &sops, 1);
}

void wSleep(int sem1_id){
    semSetup(sem1_id,'w');
    sleep(1);
}

int main(int argc , char *argv[])
{
    int sem1_id = atoi(argv[1]);
    int check_VZ_1 = atoi(argv[2]);
    int pipe = atoi(argv[3]);

    char *shm1;

    if ((shm1 = (char*)shmat(check_VZ_1, NULL, 0)) == NULL) {
        perror("shmat failed");
        return 1;
    }

    char buffer[151];
    int text;

    semSetup(sem1_id,'2');
    for (int i = 0;i < 10;i++) {
        text = 0;
        wSleep(sem1_id);
        do{
            read(pipe,&buffer[text++],1);
        }while(buffer[text - 1] != '\n');
        buffer[strlen(buffer)] = '\0';
        printf("T:Send word %d %s\n",i,buffer);
        strcpy(shm1, buffer);
        semSetup(sem1_id,'1');
    }
    return 0;
}
