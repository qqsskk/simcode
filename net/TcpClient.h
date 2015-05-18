#ifndef SIMCODE_NET_TCP_CLIENT_H
#define SIMCODE_NET_TCP_CLIENT_H
#include <simcode/net/EventLoop.h>
#include <simcode/net/Connector.h>
#include <simcode/net/TcpConnection.h>
namespace simcode
{
namespace net
{
class TcpClient
{
public:
    typedef simex::function<void (const TcpConnectionPtr&, std::string* msg)> MessageCallback;
    typedef simex::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef simex::function<void (const TcpConnectionPtr&)> CloseCallback;
    TcpClient(EventLoop* loop, const SockAddr& addr, const std::string& name);
    void active();
    void send(const char* data, size_t len);
    void setMessageCallback(const MessageCallback& c)
    {
        messageCallback_ = c;
    }
    void setConnectionCallback(const ConnectionCallback& b)
    {
        connectionCallback_ = b;
    }
    void setCloseCallback(const CloseCallback& b)
    {
        closeCallback_ = b;
    }

    EventLoop* getLoop() const
    {
        return loop_;
    }

private:
    void onConnect();
    void onClose(const TcpConnectionPtr&);
    EventLoop* loop_;
    Connector connector_;
    TcpConnectionPtr conn_;
    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
    CloseCallback closeCallback_;
    Mutex mutex_;
};
}
}
#endif