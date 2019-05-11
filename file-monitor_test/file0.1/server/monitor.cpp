#include "monitor.h"
int setnonblocking(int fd)
{
    int old_option = fcntl(fd ,F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl( fd, F_SETFL ,new_option);
    return old_option;
}
void addfd( int epollfd,int fd ,bool one_shot)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET | EPOLLRDHUP;
    if(one_shot)
    {
        event.events |= EPOLLONESHOT;
    }
    epoll_ctl( epollfd, EPOLL_CTL_ADD, fd ,&event );
    setnonblocking( fd );
}
void removefd(int epollfd, int fd)
{
    epoll_ctl( epollfd, EPOLL_CTL_DEL , fd ,0 );
    close(fd);
}
void modfd(int epollfd, int fd,int ev)
{
    epoll_event event;
    event.data.fd = fd;
    event.events = ev | EPOLLET | EPOLLONESHOT | EPOLLRDHUP;
    epoll_ctl(epollfd ,EPOLL_CTL_MOD ,fd ,&event );
}
void monitor::close_mon( bool real_close )
{
    if(real_close && (m_sockfd != -1) )
    {
        removefd( m_epollfd,m_sockfd );
        m_sockfd = -1;
        delete masg; //回收消息体
    }
}

int monitor::m_epollfd = -1;

void monitor::init(int sockfd, const sockaddr_in& addr)
{
    m_sockfd = sockfd;
    m_address = addr;
    addfd( m_epollfd, sockfd, true);

    init();
}
void monitor::init()
{
    masg->sign = 0;
    memset(masg->mac_addr,'\0',MAC_ADD_SIZE);
    memset(masg->file_name,'\0',FILE_NAME_LEN);
    memset(masg->data,'\0',BUFF_SIZE);
    memset(backup_f_name,'\0',BACKUP_F_NAME);

}
bool monitor::recv_masg()
{
    while (1)
    {
        int count = 0;
        count =recv(m_sockfd, &masg+count, sizeof(TAG)-count, MSG_WAITALL);
        if(count==-1)
        {
            if(errno == EAGAIN || errno == EWOULDBLOCK )
            {
                break;
            }
            return false;
        }
        if(count == 0)
        {
            std::cout << "something wrong in recv" << std::endl;
            return false;
        }
        //if(masg->sign == 1)
        //backup();
        //return 1;   
        //else if(masg->sign == 0)
        //recover();
        //return 0; 
    }  
    return true;
}
void monitor::backup()
{   
    memcpy(backup_f_name,masg->mac_addr,MAC_ADD_SIZE);
    strcat(backup_f_name,":");
    strcat(backup_f_name,masg->file_name);
    std::cout << "backup_file_name:" << backup_f_name << std::endl;
    if((fd = open(backup_f_name,O_CREAT|O_APPEND|O_WRONLY,S_IRUSR|S_IWUSR))==-1)
    {
        perror("open");
    }else{
        printf("success\n");
    }
    //printf("%s\n",masg->data);
    int count =write(fd,masg->data,BUFF_SIZE);
    //printf("count_write = %d\n masglenth = %d\n",count,masg->lenth);
    close(fd);


}
void monitor::recover()
{
    memcpy(backup_f_name,masg->mac_addr,MAC_ADD_SIZE);
    strcat(backup_f_name,":");
    strcat(backup_f_name,masg->file_name);
    std::cout << "backup_file_name:" << backup_f_name << std::endl;
    if((fd = open(backup_f_name,O_CREAT|O_APPEND|O_WRONLY,S_IRUSR|S_IWUSR))==-1)
    {
        perror("open");
    }else{
        printf("success\n");
    }
    //printf("%s\n",masg->data);
    int count =read(fd,masg->data,BUFF_SIZE); //循环读
    //printf("count_write = %d\n masglenth = %d\n",count,masg->lenth);
    send(m_sockfd,masg,sizeof(TAG),0);

    close(fd);
}
void monitor::process()
{
    if(masg->sign==1)
        backup();
    else if(masg->sign ==2)
        recover();

}