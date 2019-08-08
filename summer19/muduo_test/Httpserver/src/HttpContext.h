#include "HttpRequest.h"
//#include "copyable.h"

class Buffer;

class HttpContext // public copyable
{
public:
    enum HttpRequestParseState
    {
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };

    HttpContext()
        :state_(kExpectRequestLine) {}
    
    bool parseRequest(Buffer* buf,int receivetime); //XXXX
    //解析请求

    bool gotAll() const //判断状态是否解析完
    { return state_ == kGotAll; }

    void reset()  //清空(重置)请求
    {
        state_ = kExpectRequestLine;
        HttpRequest dummy;
        request_.swap(dummy);
    }

    const HttpRequest& request() const //返回指向request的引用
    { return request_; }

    HttpRequest& request()
    { return request_; }
private:
    bool processRequestLine(const char* begin,const char* end);

    HttpRequestParseState state_;
    HttpRequest request_;
};