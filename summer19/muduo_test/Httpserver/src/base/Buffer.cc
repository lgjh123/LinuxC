#include "Buffer.h"
#include "../net/SocketsOps.h"

#include <errno.h>
#include <memory.h>
#include <sys/uio.h>
#include <boost/implicit_cast.hpp>

const char Buffer::kCRLF[] = "\r\n";

ssize_t Buffer::readFd(int fd,int* savedErrno)
{
    char extrabuf[65536];     //64kB+1  多一个byte？
    struct iovec vec[2];
    const size_t writable = writableBytes();
    vec[0].iov_base = begin() + writerIndex_;
    vec[0].iov_len = writable;
    vec[1].iov_base = extrabuf;
    vec[1].iov_len = sizeof extrabuf;
    const ssize_t n = readv(fd, vec,2);
    if( n < 0 )
    {
        *savedErrno = errno;
    }else if(boost::implicit_cast<size_t>(n) <= writable) {
        writerIndex_ += n;
    }else{
        writerIndex_ = buffer_.size();
        //将写缓冲区索引改为buffer的大小，buffer的末尾，因为后面会调用append
        //他起始写的位置应该是原buffer的末尾
        append(extrabuf , n - writable);
        //将多余出来的内容append到buffer中
    }

    return n;
}
