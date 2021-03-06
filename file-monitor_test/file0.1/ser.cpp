#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <cassert>
#include <sys/epoll.h>

#define MAX_FD 65536    
#define MAX_EVENT_NUMBER 10000
int setnonblocking(int fd)
{
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL,new_option);
    return old_option;
}
int addfd(int epollfd,int fd ,bool one_shot)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if(one_shot)
    {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl(epollfd,EPOLL_CTL_ADD,fd ,&event );
    setnonblocking(fd);
}
int removefd(int epollfd, int fd)
{
    epoll_ctl( epollfd,EPOLL_CTL_DEL, fd ,0 );
    close(fd);
}

int main(int argc,char **argv)
{
    if(argc <= 2)
    {
        printf("errno ... ...\n");
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    int listenfd = socket(PF_INET, SOCK_STREAM, 0);
    assert(listenfd >= 0);
    struct linger tmp = { 1 , 0 };
    setsockopt(listenfd,SOL_SOCKET,SO_LINGER,&tmp,sizeof(tmp));

    int ret = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address ) );
    address.sin_family = AF_INET;
    inet_pton(AF_INET,ip,&address.sin_addr );
    address.sin_port = htons(port);
    
    ret = bind(listenfd, (struct sockaddr*)&address, sizeof(address ) );
    assert(ret >= 0);

    ret = listen(listenfd,5 );
    assert(ret >= 0);

    epoll_event events[MAX_EVENT_NUMBER];
    int epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd( epollfd, listenfd, false );

    while(true)
    {
        int number = epoll_wait(epollfd,events,MAX_EVENT_NUMBER, -1);
        if( (number < 0)&& (errno != EINTR) )
        {
            printf("epoll failure\n");
            break;
        }

        for( int i = 0; i < number;i++)
        {
            int sockfd = events[i].data.fd;
            if(sockfd == listenfd )
            {
                struct sockaddr_in client_address;
                socklen_t client_addrlength = sizeof(client_address);
                int connfd = accept(listenfd,(struct sockaddr*)&client_address,
                                    &client_addrlength );
                if(connfd < 0)
                {
                    printf("errno is:%d\n",errno);
                    continue;
                }
                //(if 到达最大文件数)
                //server is busy
                //countinue
                
                //初始化
            }
            else if(events[i].events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR ) )
            {
                //如果出现异常
                close(sockfd);  
            }
            else if(events[i].events & EPOLLIN )
            {
                //如果是EPOLLIN 读
            }
            else if(events[i].events & EPOLLOUT )
            {
                //EPOLLOUT 写
            }
            else
            { }
            
        }
    }
    close(epollfd);
    close(listenfd);
    return 0;

}