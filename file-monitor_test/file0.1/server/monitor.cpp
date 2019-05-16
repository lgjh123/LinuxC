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
    std::cout << "this is init" << std::endl;
    masg->sign = 0;
    masg->lenth = 0;
    memset(masg->mac_addr,'\0',MAC_ADD_SIZE);
    memset(masg->file_name,'\0',FILE_NAME_LEN);
    memset(masg->data,'\0',BUFF_SIZE);
    memset(masg->event,'\0',EVENT_BUFF);
    memset(backup_f_name,'\0',BACKUP_F_NAME);

}
bool monitor::recv_masg()
{
   
    while (1)
    {   
        printf(">>While bigan\n");
        int count = 0;
        std::cout << ">>sizeof(TAG) :"<< sizeof(TAG) << std::endl;
        //count =recv(m_sockfd, masg+count, sizeof(TAG)-count, MSG_WAITALL);
        count =recv(m_sockfd, masg, sizeof(TAG), 0);
        std::cout << ">>recv_count = " << count << std::endl;
        if(count==-1)
        {
            //std::cout<< strerror(errno) << std::endl;
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
        std::cout << ">>recv succses!\n" << std::endl;
        std::cout << "data :"<< masg->data << std::endl;
        std::cout << "event :"<< masg->event << std::endl;
        std::cout << "filename :"<< masg->file_name << std::endl;
        std::cout << "lenth :"<< masg->lenth << std::endl;
        std::cout << "mac_addr :"<< masg->mac_addr << std::endl;
        std::cout << "sign :"<< masg->sign << std::endl;
        printf("\n");
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
    printf(">>IN backup() before memecpy\n");
    transf_file_name();
    mkdir(masg->mac_addr,S_IRWXU);
    memcpy(backup_f_name,masg->mac_addr,MAC_ADD_SIZE);
    strcat(backup_f_name,"/");
    strcat(backup_f_name,masg->file_name);
    std::cout << "backup_file_name:" << backup_f_name << std::endl;
    printf(">>IN backup before open()\n");
    if((fd = open(backup_f_name,O_CREAT|O_APPEND|O_WRONLY,S_IRUSR|S_IWUSR))==-1)
    {
        perror("open");
    }else{
        printf(">>backip_open_success\n");
    }
   // //printf("%s\n",masg->data);
   // int count =write(fd,masg->data,BUFF_SIZE);
   // printf(">>count_write = %d\n masglenth = %d\n",count,masg->lenth);
   // printf(">>write_succes\n");
   // close(fd);
    strcat(masg->data,"\0");
    std::ofstream out;
    out.open(backup_f_name,std::ios::app);
    out << masg->data;
    out.close();

}
void monitor::recover()
{
    memcpy(backup_f_name,masg->mac_addr,MAC_ADD_SIZE);
    strcat(backup_f_name,"/");
    transf_file_name(); //目录内容转换
    strcat(backup_f_name,masg->file_name);
    std::cout << "backup_file_name:" << backup_f_name << std::endl;
    if((fd = open(backup_f_name,O_CREAT|O_APPEND|O_WRONLY,S_IRUSR|S_IWUSR))==-1)
    {
        perror("open");
    }else{
        printf("recover_open_success\n");
    }
    //printf("%s\n",masg->data);
    int count =read(fd,masg->data,BUFF_SIZE); //循环读
    recover_file_name();
    //printf("count_write = %d\n masglenth = %d\n",count,masg->lenth);
    masg->lenth = get_file_size();
    std::cout << ">>read succses!\n" << std::endl;
    std::cout << ">>send>>data :"<< masg->data << std::endl;
    std::cout << ">>send>>event :"<< masg->event << std::endl;
    std::cout << ">>send>>filename :"<< masg->file_name << std::endl;
    std::cout << ">>send>>lenth :"<< masg->lenth << std::endl;
    std::cout << ">>send>>mac_addr :"<< masg->mac_addr << std::endl;
    std::cout << ">>send>>sign :"<< masg->sign << std::endl;

    send(m_sockfd,masg,sizeof(TAG),0);
    masg->lenth = 0;
    send(m_sockfd,masg,sizeof(TAG),0);
    

    close(fd);
}
void monitor::process()
{
    printf("IN process()\n");
    std::cout << masg->sign << std::endl;
    if(masg->sign==0)
        backup();
    else if(masg->sign ==1)
        recover();

}
void monitor::transf_file_name()
{
    for(auto &c : masg->file_name)
    {
        if(c == '/')
        {
            c = '@';
        }
    }
}
void monitor::recover_file_name()
{
    for(auto &c : masg->file_name)
    {
        if(c == '@')
        {
            c = '/';
        }
    }
}



unsigned long monitor::get_file_size()
{
    unsigned long filesize = -1;
    struct stat statbuff;
    if (stat(backup_f_name, &statbuff) < 0)
    {
        return filesize;
    }
    else
    {
        filesize = statbuff.st_size;
    }
    return filesize;
}
