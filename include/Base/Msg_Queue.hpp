#ifndef MSG_QUEUE_H
#define MSG_QUEUE_H

#include <iostream>

#include <chrono>
#include <condition_variable>
#include <list>
#include <memory>
#include <mutex>

// 消息队列

template<class T>
class CMsgQueue
{
public:

    void Clear()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        mMsgList.clear();
    }

    bool Push(const std::shared_ptr<T> &msg)
    {
        if ( !msg ) {
            return false;
        }

        std::unique_lock<std::mutex> lock(mMutex);

        mMsgList.push_back(msg);

        mCond.notify_all();

        return true;
    }

    std::shared_ptr<T> TryPop()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        if ( mMsgList.empty() ) {
            return std::shared_ptr<T>();
        }

        std::shared_ptr<T> first = mMsgList.front();
        mMsgList.pop_front();
        return first;
    }

    std::shared_ptr<T> Pop()
    {
        std::unique_lock<std::mutex> lock(mMutex);

        if ( mMsgList.empty() ) {
            mCond.wait(lock);
        }

        if( !mMsgList.empty() ) {
            std::shared_ptr<T> first = mMsgList.front();
            mMsgList.pop_front();
            return first;
        }

        return std::shared_ptr<T>();
    }

    std::shared_ptr<T> Pop(uint32_t msec)
    {
        std::unique_lock<std::mutex> lock(mMutex);

        if ( mMsgList.empty() ) {
            mCond.wait_for(lock, std::chrono::milliseconds(msec));
        }

        if( !mMsgList.empty() ) {
            std::shared_ptr<T> first = mMsgList.front();
            mMsgList.pop_front();
            return first;
        }

        return std::shared_ptr<T>();
    }

    void Wakeup()
    {
        mCond.notify_all();
    }

    uint32_t Size() const
    {
        std::unique_lock<std::mutex> lock(mMutex);

        return mMsgList.size();
    }

    bool Empty() const
    {
        std::unique_lock<std::mutex> lock(mMutex);

        return mMsgList.empty();
    }

private:
    std::mutex mMutex;
    std::condition_variable mCond;
    std::list<std::shared_ptr<T> > mMsgList;
};

#endif // MSG_QUEUE_H
