# mymuduo

## 
```
1. 剔除boost库依赖
```

### 实现思路
```
一个 EventLoop 可以有多个 Channel 但 一个Channel 只能有一个 EventLoop 
一个 EventLoop 可以有多个 Poller 但 Poller 只能有一个 EventLoop 

Channel: 表示一个文件描述符和对应事件的关系,与特定的文件描述符相关联，负责注册读写事件以及处理事件发生时的回调
    1.  封装了 fd events(注册的事件) revents(发生的具体事件) 
    2.  提供了方法 
        (1) 设置fd相应的事件状态 enableReading() disableReading() enableWriting() disableWriting() disableAll() 
        (2) fd得到Poller通知后,处理事件回调 handleEvent()

EpollPoller: 使用 epoll 系统调用来监听文件描述符上的事件，当事件就绪时，EpollPoller 会通知相应的 Channel    
    1.  封装了 ChannelMap (key:fd value:Channel*)
    2.  提供了方法 
        (1) 设置监听channel - fd 
        (2) 给channel - fd 注册 或 修改 事件 updateChannel()
        (3) 删除channel - fd上注册的事件 removeChannel()

EventLoop: 是事件循环的核心部分，负责事件的分发和处理。
```