#ifndef SIMCODE_NET_TCPCONNECTION_H
#define SIMCODE_NET_TCPCONNECTION_H
#include <simcode/net/SockAddr.h>
#include <simcode/net/Socket.h>
#include <simcode/net/EventLoop.h>
#include <simcode/base/typedef.h>
#include <simcode/base/noncopyable.h>

namespace simcode
{
namespace net
{
class TcpConnection;
typedef simex::shared_ptr<TcpConnection> TcpConnectionPtr;
class TcpConnection : noncopyable,
    public simex::enable_shared_from_this<TcpConnection>
{
public:
    typedef simex::function<void()> CloseCallback;
    typedef simex::function<void(const TcpConnectionPtr&, std::string* msg)> MessageCallback;
    TcpConnection(EventLoop* loop, int connfd, const SockAddr& peerAddr);
    void send(const char* data, size_t len);
    int connfd() const
    {
        return socket_.sockfd();
    }
    const SockAddr& peerAddr()
    {
        return peerAddr_;
    }
    void setCloseCallback(const CloseCallback& b)
    {
        closeCallback_ = b;
    }
    void setMessageCallback(const MessageCallback& b)
    {
        messageCallback_ = b;
    }
    void setContext(const simex::any& c)
    {
        context_ = c;
    }
    const simex::any& getContext() const
    {
        return context_;
    }
    simex::any* getMutableContext()
    {
        return &context_;
    }
    int errcode() const
    {
        return errcode_;
    }
    bool isWriting() const
    {
        return events_ & EPOLLOUT;
    }
    bool isReading() const
    {
        return revents_ & EPOLLIN;
    }
    void setEvenst(int events)
    {
        revents_ = events;
    }
private:
    void eventHandle(int events);
    void onClose();
    void enableWriting()
    {
         events_ |= EPOLLOUT; 
         update();
    }
    void disableWriting()
    {
        events_ &= ~EPOLLOUT;
        update();
    }
    void handleRead();
    void handleWrite();
    void update();
private:
    EventLoop* loop_;
    Socket socket_;
    SockAddr peerAddr_;
    CloseCallback closeCallback_;
    MessageCallback messageCallback_;
    std::string readBuf_;
    std::string writeBuf_;
    int errcode_; //use to save errno
    int events_;
    int revents_;
    Mutex mutex_;
    simex::any context_;
};

}
}

#endif