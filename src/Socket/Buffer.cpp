#include "Buffer.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/uio.h>
#include <errno.h>

const char Buffer::kCRLF[] = "\r\n";

Buffer::Buffer(size_t init_size) : 
    buffer_(k_cheap_prepend + init_size),
    read_index_(k_cheap_prepend),
    write_index_(k_cheap_prepend) {
    assert(readable_bytes() == 0);
    assert(writable_bytes() == init_size);
    assert(prependable_bytes() == k_cheap_prepend);
}

ssize_t Buffer::readfd(int fd, int* save_errno) {
    char extra_buffer[65535] = {0}; // 栈上的内存
    struct iovec vec[2];
    const size_t writeable = writable_bytes();

    // iov_base --> 起始地址
    // iov_len --> 字长
    vec[0].iov_base = begin() + write_index_;
    vec[0].iov_len = writeable;

    vec[1].iov_base = extra_buffer;
    vec[1].iov_len = sizeof(extra_buffer);

    const int iovcnt = writeable < sizeof(extra_buffer) ? 2 : 1;
    const ssize_t ret = readv(fd, vec, iovcnt);
    if (ret < 0) {
        *save_errno = errno;
    } else if (ret <= writeable) {
        write_index_ += ret;
    } else { // extra_buffer中也有数据
        write_index_ =  buffer_.size();
        // 缓冲区写入数据并进行扩容
        append(extra_buffer, ret-writeable);
    }
    return ret;
}

ssize_t Buffer::writefd(int fd, int* save_errno) {
    ssize_t ret = write(fd, peek(), readable_bytes());
    if (ret < 0) {
        *save_errno = errno;
    }
    return ret;
}