#ifndef SIMCODE_NET_EVENT_LOOP_THREAD_H
#define SIMCODE_NET_EVENT_LOOP_THREAD_H
#include <simcode/net/EventLoop.h>
namespace simcode
{
namespace net
{
class EventLoopThread : noncopyable
{
public:
    typedef simex::function<void(EventLoop*)> ThreadInitCallback;
	EventLoopThread(const ThreadInitCallback& cb =ThreadInitCallback());
    EventLoop *startLoop();
private:
    SharedPtr<EventLoop> loop_;
    SimThreadPtr thread_;
    ThreadInitCallback callback_;
};
}
}

#endif
