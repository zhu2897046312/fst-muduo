#include "Poller.h"
#include <cstdlib>

namespace fst {

//EentLoop可以通过该接口获取默认的IO复用的具体实现
Poller* Poller::newDefaltPoller(EventLoop *loop){
    if(::getenv("MUDUO_USE_POLL")){
        return nullptr;
    }else{
        return nullptr;
    }
}

}