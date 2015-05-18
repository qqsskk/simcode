#ifndef UDP_CLIENT_H
#define UDP_CLIENT_H

#include <simcode/net/UdpConnection.h>
#include <simcode/net/EventLoop.h>
#include <simcode/base/noncopyable.h>
#include <simcode/net/Socket.h>
namespace simcode
{
namespace net
{

class UdpClient : noncopyable
{
public:
    typedef simex::function<void (const UdpConnectionPtr&, std::string* msg)> MessageCallback;
    UdpClient(EventLoop*, const SockAddr&, const std::string& name);
    int Send(const char* buf, size_t len);
    void setMessageCallback(const MessageCallback& c)
    {
        messageCallback_ = c;
    }
    UdpConnectionPtr getConn() const
    {
        return conn_;
    }
private:
    void eventHandle();
    void onMessage(const UdpConnectionPtr& c, std::string* msg);
private:
    Socket socket_;
    UdpConnectionPtr conn_;
    MessageCallback messageCallback_;
};

}
}

#endif