// Disabling Copy Functions and Copy Operators
// Example:
// class demo: NoCopyable {
//
// }

#ifndef NONCOPYABLE_H_
#define NONCOPYABLE_H_

class NoCopyable {
    protected:
        NoCopyable() = default;
        ~NoCopyable() = default;
        NoCopyable(const NoCopyable& rhs) = delete;
        NoCopyable& operator=(const NoCopyable& rhs) = delete;
};

#endif