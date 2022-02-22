/*** 
 * @Author: Zty
 * @Date: 2022-02-14 12:57:50
 * @LastEditTime: 2022-02-19 16:05:40
 * @LastEditors: Zty
 * @Description:  缓冲区设计
 * @FilePath: /multhread/src/Socket/Buffer.hpp
 */

#ifndef BUFFER_H_
#define BUFFER_H_


#include <vector>
#include <string>
#include <algorithm>
#include "../Base/NonCopyable.hpp"

class Buffer : NoCopyable {
    public:
        explicit Buffer(size_t init_size = k_init_size);

        // 可读长度
        size_t  readable_bytes() const {
            return write_index_ - read_index_;
        }

        // 可写长度
        size_t writable_bytes()  const {
            return buffer_.size() - write_index_;
        }

        // 返回头长度 
        size_t prependable_bytes() const  {
            return read_index_;
        }

        // 返回缓冲区可读数据起始地址
        const char* peek() const {
            return begin() + read_index_;
        }
        
        // 缓冲区偏移
        void retrieve(size_t len) {
            // 读取部分
            if (len < readable_bytes()) {
                read_index_ += len;
            } else {
                retrieve_all();
            }
        }
        
        // 缓冲区复位
        void retrieve_all() {
            read_index_ = write_index_ = k_cheap_prepend;
        }

        // 读取所有信息
        std::string retrieve_asString() {
            return retrieve_as_string(readable_bytes());
        }

        // 读取len长度数据
        std::string retrieve_as_string(size_t len) {
            std::string res(peek(), len);
            retrieve(len); // 完成读操作后，复位缓冲区
            return res;
        }

        void retrieveUntil(const char* end) {
            retrieve(end-peek());
        }

        // 保证缓冲区有足够长度可写
        void ensure_writable_bytes(size_t len) {
            if (writable_bytes() < len) {
                makespace(len);
            }
        }

        // 返回可写数据地址
        char* begin_write() {
            return begin() + write_index_;
        }

        const char* begin_write()  const {
            return begin() + write_index_;
        }

        void append(const std::string&  str) {
            append(str.c_str(), str.size());
        }

        // 往缓冲区中添加数据
        void append(const char* data, size_t len) {
            ensure_writable_bytes(len);
            std::copy(data, data+len, begin_write());
            write_index_ += len;
        }

        // 从fd中读取数据
        ssize_t readfd(int fd, int *save_errno);

        // 通过fd发送数据
        ssize_t writefd(int fd, int* save_errno);

        const char* findCRLF () const {
            const char* crlf = std::search(peek(), begin_write(), kCRLF, kCRLF+2);
            return crlf == NULL ? NULL : crlf;
        }

    private:
        // 缓冲区起始地址
        char* begin() {
            return &*buffer_.begin();
        }

        const char* begin() const {
            return &*buffer_.begin();
        }

        // 扩容
        void makespace(size_t len) {
            if (writable_bytes() + prependable_bytes() < len + k_cheap_prepend) {
                buffer_.resize(write_index_ + len);
            } else {
                size_t readable = readable_bytes();
                std::copy(begin() + read_index_, begin() + write_index_, begin() + k_cheap_prepend);
                
                read_index_ = k_cheap_prepend;
                write_index_ = read_index_ + readable;
            }
        }

    public:
        static const size_t k_cheap_prepend = 8;
        static const size_t k_init_size = 1024;

    private:
        std::vector<char> buffer_;
        size_t read_index_;
        size_t write_index_;
        static const char kCRLF[];
};



#endif