#pragma once

#include "noncopyable.h"
#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <sys/types.h>
#include <thread>
#include <string>
namespace fst {

class Thread : nonccopyable
{
public:
    using ThreadFunc = std::function<void()>;
    explicit Thread(ThreadFunc, const std::string &name = "");
    ~Thread();

    void start();
    void join();

    inline bool started() const { return started_; }
    inline pid_t tid() const { return tid_; }
    inline const std::string& name() const { return name_; }

    inline static int nmuCreated() { return numCreated_; }
private:
    void setDefaultName();

private:
    bool started_;
    bool joined_;
    std::shared_ptr<std::thread> thread_;
    pid_t tid_;
    ThreadFunc func_;
    std::string name_;
    static std::atomic_int numCreated_;
};

}