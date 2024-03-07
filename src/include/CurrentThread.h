#pragma once

#include <sched.h>
namespace fst {

static thread_local pid_t t_cachedTid;

void cacheTid();

inline pid_t tid(){
    if(__builtin_expect(t_cachedTid == 0, 0)){
        cacheTid();
    }
    return t_cachedTid;
}

}