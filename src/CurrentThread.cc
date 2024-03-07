#include"CurrentThread.h"
#include <sched.h>
#include <unistd.h>
#include <sys/syscall.h>

namespace fst {

void cacheTid(){
    if(t_cachedTid == 0){
        t_cachedTid = static_cast<pid_t>(::syscall(SYS_gettid));
    }
}

}