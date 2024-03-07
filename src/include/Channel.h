#pragma once

#include "Timestamp.h"
#include "EventLoop.h"
#include "noncopyable.h"
#include <algorithm>
#include <functional>
#include <memory>
namespace fst {

class EventLoop;

// one loop  ===> listen some channel
// one channel ===> belong to one loop
/**
 * 重点理清楚: EventLoop \ Channel \ Poller 之间的关系  <=== Reactot模型上对应的关系 Demultiplex事件分发器
 *                      
 */

class Channel : nonccopyable
{
public:
    using EventCallBack = std::function<void()>;
    using ReadEventCallBack = std::function<void(Timestamp)>;

    explicit Channel(EventLoop *loop, int fd);
    ~Channel();

    // fd得到Poller通知后,处理事件
    void handleEvent(Timestamp receiveTime);

    void remove();
    
    // 设置回调函数对象
    void setReadCallBack(ReadEventCallBack cb) { readCallBack_ = std::move(cb); }
    void setWriteCallBack(EventCallBack cb) { writeCallBack_ = std::move(cb); }
    void setCloseCallBack(EventCallBack cb) { closeCallBack_ = std::move(cb); }
    void setErrorCallBack(EventCallBack cb) { errorCallBack_ = std::move(cb); }

    // 防止Channel被手动remove掉,Channel还在执行回调操作
    void tie(const std::shared_ptr<void>&);

    int fd() const { return fd_; }
    int events() const { return events_; }
    void set_revents(int revents) { revents_ = revents; }

    // 设置fd相应的事件状态
    void enableReading() { events_ |= kReadEvent; update(); }
    void disableReading() { events_ &= ~kReadEvent; update(); }
    void enableWriting() { events_ |= kWriteEvent; update(); }
    void disableWriting() { events_ &= ~kWriteEvent; update(); }
    void disableAll() { events_ = kNoneEvent; update(); }

    //返回fd的当前事件状态
    bool isNoneEvent() const { return events_ == kNoneEvent; }
    bool isWriting() const { return events_ & kWriteEvent; }
    bool isReading() const {return events_ & kReadEvent; }

    int index() {return index_; }
    void set_index(int idx) { index_ = idx; }

    //
    EventLoop* ownerLoop() { return loop_; }

private:
    void update();
    void handleEventWithGuard(Timestamp receiveTime);

private:
    static const int kNoneEvent;
    static const int kReadEvent;
    static const int kWriteEvent;

    EventLoop *loop_;   // 事件循环
    const int fd_;      // fd, Poller监听对象
    int events_;        // 注册监听事件
    int revents_;       // Poller返回具体发生的事件
    int index_;         

    std::weak_ptr<void> tie_;
    bool tied_;

    ReadEventCallBack readCallBack_;
    EventCallBack writeCallBack_;
    EventCallBack closeCallBack_;
    EventCallBack errorCallBack_;
};


}