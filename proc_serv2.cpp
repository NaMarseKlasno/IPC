#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <signal.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(int argc, char* argv[]){
    struct sockaddr_in server_addr;
    struct sockaddr_in client;

    int socketfd;
    int txt;
    int len_client;
    int check;
    char buff[151];

    if((socketfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        exit(1);

    bzero((char*)&server_addr, sizeof(server_addr));
    bzero((char*)&client, sizeof(client));

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons((unsigned int)atoi(argv[1]));

    if(bind(socketfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) != 0)
        exit(1);

    txt = open("serv2.txt", O_CREAT | O_WRONLY, 0666);
    if(txt == -1)
        exit(1);
    len_client = sizeof(client);

    check = 0;
    while(check < 10){
        if(recvfrom(socketfd, buff, 151, MSG_WAITALL, (struct sockaddr*)&client, (socklen_t*)&len_client) == -1)
            exit(1);

        ssize_t write_check;
        write_check = write(txt, &buff, strlen(buff));
        if(write_check == -1)
            exit(1);
        write_check = write(txt, "\n", 1);
        if(write_check == -1)
            exit(1);

        check++;
    }

    return 0;
}