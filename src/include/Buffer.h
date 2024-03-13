#pragma once

#include <algorithm>
#include <bits/types/struct_iovec.h>
#include <cerrno>
#include <cstddef>
#include <string>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <sys/uio.h>

namespace fst {

class Buffer 
{
public:
    static const size_t kCheaPrepend = 8;
    static const size_t kInitialSize = 1024;

    explicit Buffer(size_t initialSize = kInitialSize)
        : buffer_(kCheaPrepend + initialSize)
        , reader_index_(kCheaPrepend)
        , write_index_(kCheaPrepend){}
    
    size_t readableBytes() const{ return write_index_ - reader_index_; }
    size_t writeBytes() const { return buffer_.size() - write_index_; }
    size_t prependableBytes() const { return reader_index_; }

    const char* peek() const { return begin() + reader_index_; }

    void retrieve(size_t len) {
        if(len < readableBytes()){
            reader_index_ += len;
        }else{
            retrieveAll();
        }
    }
    void retrieveAll(){ reader_index_ = write_index_ = kCheaPrepend; }
    std::string retrieveAllAsString(){ return retrieveAsString(readableBytes()); }
    std::string retrieveAsString(size_t len){
        std::string result(peek(), len);
        retrieve(len);
        return result;
    }

    void ensureWriteableBytes(size_t len){
        if(write_index_ < len){
            makeSpace(len);
        }
    }

    void append(const char* data, size_t len){
        ensureWriteableBytes(len);
        std::copy(data, data + len,  beginWrite());
        write_index_ += len;
    }

    ssize_t readFd( int fd, int *saveErrno)
    { 
        char extrabuf[65536] = {0};
        struct iovec vec[2];
        const size_t writable = writeBytes();
        vec[0].iov_base = begin() + write_index_;
        vec[0].iov_len = writable;

        vec[1].iov_base = extrabuf;
        vec[1].iov_len = sizeof(extrabuf);

        const int iovcnt = (writable < sizeof(extrabuf)) ? 2 : 1;
        const ssize_t n = ::readv(fd, vec, iovcnt);
        if(n < 0){
            *saveErrno = errno;
        }else if(n <= writable){
            write_index_ += n;
        }else{
            write_index_ = buffer_.size();
            append(extrabuf, n - writable);
        }
        return n;
    }

    ssize_t writeFd(int fd,int *saveErrno){
        ssize_t n = ::write(fd, peek(), readableBytes());
        if(n < 0){
            *saveErrno = errno;
        }
        return n;
    }

    char* beginWrite(){ return begin() + write_index_; }
private:
    char* begin(){ return &*buffer_.begin(); }
    char* end() { return &*buffer_.end(); }
    const char* begin() const { return &*buffer_.begin(); }

    void makeSpace(size_t len){
        if(writeBytes() + prependableBytes() < len + kCheaPrepend){
            buffer_.resize(write_index_ + len);
        }else{
            size_t readable = readableBytes();
            std::copy(begin() + reader_index_
                    , begin() + write_index_
                    , begin() + kCheaPrepend);
            reader_index_ = kCheaPrepend;
            write_index_ = reader_index_ + readable;
        }
    }
private:
    std::vector<char> buffer_;
    size_t reader_index_;
    size_t write_index_;
};

}