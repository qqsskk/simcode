#ifndef SIMCODE_NET_EVENTLOOP_THREAD_POOL_H
#define SIMCODE_NET_EVENTLOOP_THREAD_POOL_H
#include <simcode/net/EventLoopThread.h>
#include <atomic>
namespace simcode
{
namespace net
{
class EventLoopThreadPool
{
public:
    typedef simex::shared_ptr<EventLoopThread> EventLoopThreadPtr;
    EventLoopThreadPool():
        threadNum_(0)
    {
    }
    EventLoop* getNextLoop();
    void start();
    void setThreadNum(int n)
    {
        threadNum_ = n;
    }
private:
    std::vector<EventLoopThreadPtr> loopThreadPool_;
    std::vector<EventLoop*> loopPool_;
    uint16_t threadNum_;
    std::atomic_uint next_;
};
}
}
#endif
