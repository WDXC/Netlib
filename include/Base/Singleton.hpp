/*** 
 * @Author: Zty
 * @Date: 2022-02-13 10:23:57
 * @LastEditTime: 2022-02-13 10:28:13
 * @LastEditors: Zty
 * @Description: 单例模板 
 * @FilePath: /multhread/Base/Singleton.hpp
 */

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