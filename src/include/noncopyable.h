#pragma once

namespace fst {
/**
 * noncopyable 被继承后,派生类对象能够政策的构造和析构
 * 但派生类对象无法被进行拷贝和赋值操作
 */
class nonccopyable
{
public:
    nonccopyable(const nonccopyable&) = delete;
    nonccopyable& operator=(const nonccopyable&) = delete;
protected:
    nonccopyable() = default;
    ~nonccopyable() = default;
};

}