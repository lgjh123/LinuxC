#pragma once

#include <algorithm>
#include <string>
#include <vector>


#include <assert.h>
/// +-------------------+------------------+------------------+
/// | prepend  bytes    |  readable bytes  |  writable bytes  |
/// |                   |     (CONTENT)    |                  |
/// +-------------------+------------------+------------------+
/// |                   |                  |                  |
/// 0      <=      readerIndex   <=   writerIndex    <=     size
class Buffer
{
public:
    static const size_t kCheapPrepend = 8;
    static const size_t kInitialSize = 1024;

    Buffer()
        : buffer_(kCheapPrepend + kInitialSize),
          readerIndex_(kCheapPrepend),
          writerIndex_(kCheapPrepend)
    {
        assert(readableBytes() == 0);
        assert(writableBytes() == kInitialSize);
        assert(prependableBytes() == kCheapPrepend);
    }
        
    void swap(Buffer& rhs)
    {
        buffer_.swap(rhs.buffer_);
        std::swap(readerIndex_,rhs.readerIndex_);
        std::swap(writerIndex_,rhs.writerIndex_);
    }

    size_t readableBytes() const
    { return writerIndex_ - readerIndex_; }

    size_t writableBytes() const
    { return buffer_.size() - writerIndex_; }

    size_t prependableBytes() const
    { return readerIndex_; }

    const char* peek() const
    { return begin() + readerIndex_; }

    void retrieve(size_t len)    //检索
    {
        assert(len <= readableBytes());
        readerIndex_ += len;
    }

    void retrieveUntil(const char* end)
    {
        assert(peek() <= end);
        assert(end <= beginWrite());
        retrieve(end - peek());
    }

    void retrieveAll()
    {
        readerIndex_ = kCheapPrepend;
        writerIndex_ = kCheapPrepend;
    }

    std::string retrieveAsString()
    {
        std::string str(peek(),readableBytes());
        retrieveAll();
        return str;
    }

    void append(const std::string& str)
    {
         append(str.data(),str.length());
    }

    void append(const char* /*restrict*/ data,size_t len)
    {
        ensureWritableBytes(len);
        std::copy(data ,data+len,beginWrite());
        hasWritten(len);
    }

    void append(const void* /*restrict*/ data, size_t len)
    {
        append(static_cast<const char*>(data),len);
    }

    void ensureWritableBytes(size_t len)
    {
        //保证可写空间足够
        if(writableBytes() < len)
        {
            makeSpace(len);
        }
        assert(writableBytes() >= len);
    }

    char* beginWrite()    //返回可写区的起始指针
    {  return begin() + writerIndex_; }
    
    const char* beginWrite() const
    { return begin() + writerIndex_; }

    void hasWritten(size_t len) //写入之后更新已写入区的起始索引
    { writerIndex_ += len; }

    void prepend(const void* /*restrict*/ data ,size_t len)
    {
        //对预留区进行操作，预留区会减小
        //用于向读缓冲区头部添加数据
        assert(len <= prependableBytes());
        readerIndex_ -= len;
        const char* d = static_cast<const char*>(data);
        std::copy(d,d+len,begin() + readerIndex_);
    }

    //prepend减小了预留区增加了可读区，shrink可以重置预留区
    void shrink(size_t reserve)    //收缩写缓冲区，并重置预留区索引
    {
        std::vector<char> buf(kCheapPrepend+readableBytes()+reserve);
        //建立一个临时的buf,预留区为8，可读区和buffer_的可读取大小相同，可写区的大小为传入的参数
        std::copy(peek(),peek()+readableBytes(),buf.begin()+kCheapPrepend);
        //将buffer_的可读区内容，复制到buf的可读区中
        buf.swap(buffer_);
        //交换buffer_和buf
    }
    

    ssize_t readFd(int fd,int* savedErrno);
    //声明读函数

private:

    char* begin()
    { return &*buffer_.begin(); }

    const char* begin() const
    { return &*buffer_.begin(); }

    void makeSpace(size_t len)
    {
        if(writableBytes() + prependableBytes() < len + kCheapPrepend)
        {
            buffer_.resize(writerIndex_ + len);
        }
        else
        {
            //把读数据向前挪，在buffer 中流出空闲
            assert(kCheapPrepend < readerIndex_);
            size_t readable = readableBytes();
            std::copy(begin() + readerIndex_,
                      begin() + writerIndex_,
                      begin() + kCheapPrepend);
            readerIndex_ = kCheapPrepend;
            writerIndex_ = readerIndex_ + readable;
            assert(readable == readableBytes());
        }
    }

private:
    std::vector<char> buffer_;
    size_t readerIndex_;
    size_t writerIndex_;

};

