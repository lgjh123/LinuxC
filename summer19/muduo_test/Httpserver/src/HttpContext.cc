#include "Buffer.h"
#include "HttpContext.h"

bool HttpContext::processRequestLine(const char* begin ,const char* end)
{
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start,end,' '); //找到空格的位置
    if(space != end && request_.setMethod(start,space))
    {
        start = space + 1;
        space = std::find(start,end,' ');  //找到下一个空格，解析URL
        if(space != end)
        {
            const char* question = std::find(start,space,'?');
            if(question != space)
            {
                request_.setPath(start,question);
                request_.setQuery(question,space);
            }
            else
            {
                request_.setPath(start, space);
            }
            start = space + 1;  //找到URL后面空格的下一位
            succeed = end - start == 8 && std::equal(start, end-1, "HTTP/1.");
            if(succeed)
            {
                if(*(end - 1) == '1')
                {
                    request_.setVersion(HttpRequest::kHttp11);
                }
                else if(*(end -1) == '0')
                {
                    request_.setVersion(HttpRequest::kHttp10);
                }
                else
                {
                    succeed = false;  //HTTP版本项不完整
                }
                
            }
            
        }
    }
    return succeed;
}

bool HttpContext::parseRequest(Buffer* buf,int receiveTime)
{
    bool ok = true;
    bool hasMore = true;
    while(hasMore)
    {
        if(state_ == kExpectRequestLine)    // 起始状态是 解析行
        {
            const char* crlf = buf -> findCRLF();  //找到一行结束符的位置
            if(crlf)
            {
                ok = processRequestLine(buf->peek(), crlf); //处理是否成功，这行是否完整
                if(ok)  //如果完整，且解析完
                {
                    buf -> retrieveUntil(crlf+2);  //增加可读取的索引，改到结束符完了的下一位，之前那行解析完
                    state_ = kExpectHeaders;   //标志为改为 解析包头
                }
                else  //如果不ok说明一行不完整,没有更多了
                {
                    hasMore = false;
                }
            }
            else  //同上
            {
                hasMore = false;
            }
        }
        else if(state_ == kExpectHeaders) //如果状态是 解析包头
        {
            const char* crlf = buf->findCRLF();
            if(crlf)
            {
                const char* colon = std::find(buf->peek(), crlf, ":"); //找到：的位置
                if(colon != crlf)  //冒号的地方不在结尾
                {
                    request_.addHeader(buf->peek(),colon,crlf);   //把这行开头到冒号，冒号到结尾传进addHxx，半开半闭(peek是得到readable的头指针)
                }
                else
                {
                    //遇到空行 请求head结束 
                    //FIXME
                    state_ = kGotAll;
                    hasMore = false;
                }
                buf->retrieveUntil(crlf + 2);
            }
            else
            {
                hasMore = false;
            }
        }
        else if(state_ == kExpectBody)
        { /*FIXME*/ }
    }
    return ok;
}