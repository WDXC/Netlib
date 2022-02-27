// a Singleton template
// Example:
// using ins = Singleton<classname>;

#ifndef SINGLETON_H_
#define SINGLETON_H_

template <class ClassType>
class Singleton {
    public:
        static ClassType* instance();
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton& ) = delete;
    private:
        Singleton() = default;
        ~Singleton() = default;
};

template <class ClassType>
ClassType* Singleton<ClassType>::instance() {
    static ClassType ins;
    return &ins;
}


#endif