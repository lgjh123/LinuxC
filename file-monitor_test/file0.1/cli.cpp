#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/msg.h>

#define BUF_SIZE 1024
struct msg_st
{
    long int msg_type;
    char text[BUF_SIZE];
};

#define BUFFER_SIZE 1024

int main(int argc ,char**argv)
{
    if( argc <= 2)
    {
        printf("erro ... ...\n");
        return 1;
    }
    const char * ip = argv[1];
    int port = atoi(argv[2]);

    struct sockaddr_in server_address;
    bzero( &server_address,sizeof( server_address ) );
    server_address.sin_family = AF_INET;
    inet_pton( AF_INET, ip ,&server_address.sin_addr ) ;
    server_address.sin_port = htons( port );

    int sockfd =socket (PF_INET, SOCK_STREAM, 0 );
    assert(sockfd >= 0);
    if (connect(sockfd,( struct sockaddr* )&server_address,sizeof( server_address )) <0)
    {
        printf("connetion failed\n");
        close(sockfd);
        return 1;
    } 
    //收消息
    int msgid = -1;
    struct msg_st data;
    long int msgtype = 0;

    //建立消息队列
    msgid = msgget((key_t)1234,0666 | IPC_CREAT);
    if(msgid == -1)
    {
        printf("msgid errno\n");
    }
    msgrcv(msgid,(void*)&data,BUF_SIZE,msgtype,0);
    printf("recv:%s\n",data.text);
    

}