#ifndef SIMCODE_NET_TCPSERVER_H
#define SIMCODE_NET_TCPSERVER_H
#include <simcode/thread/thread_safe_queue.h>
#include <simcode/net/EventLoop.h>
#include <simcode/net/SockAddr.h>
#include <simcode/net/Socket.h>
#include <simcode/net/TcpConnection.h>
#include <simcode/net/VecMap.h>
#include <simcode/net/EventLoopThreadPool.h>
#include <simcode/net/Acceptor.h>
#include <simcode/net/ConnManager.h>
namespace simcode
{
namespace net
{
typedef BaseConnManager<TcpConnectionPtr> BaseTcpConnManager;
class TcpConnMap : public BaseTcpConnManager
{
public:
    void add(uint64_t id, const TcpConnectionPtr& conn);
    TcpConnectionPtr get(uint64_t id);
    void erase(uint64_t);
private:
    Mutex mutex_;
    std::map<uint64_t, TcpConnectionPtr> connMap_;
};
class TcpServer : noncopyable
{
public:
    typedef simex::function<void (const TcpConnectionPtr&, Buffer* msg)> MessageCallback;
    typedef simex::function<void (const TcpConnectionPtr&)> ConnectionCallback;
    typedef simex::function<void (const TcpConnectionPtr&)> CloseCallback;
    TcpServer(EventLoop* loop, const SockAddr& addr, const std::string& name);
    void start();
    void setThreadNum(int n)
    {
        threadNum_ = n;
    }
    void setConnManager(const SharedPtr<BaseTcpConnManager>& m)
    {
        conntectionList_ = m;
    }
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
private:
    void onConnection(int connfd, const SockAddr& peerAddr);
    void onClose(const TcpConnectionPtr&);
    void eventHandle(int events);
    void hanleRead();
private:
    simcode::net::EventLoop* loop_;
    simcode::thread::ThreadSafeQueue queue_;
    MessageCallback messageCallback_;
    ConnectionCallback connectionCallback_;
    CloseCallback closeCallback_;
    Acceptor acceptor_;
    SharedPtr<BaseTcpConnManager> conntectionList_;
    //std::map<uint64_t, TcpConnectionPtr> conntectionList_;
    //Mutex mutex_;
    int threadNum_;
    EventLoopThreadPool loopThreadPool_;
};
}
}

#endif
