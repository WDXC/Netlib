// Version of c++11 based on boost::any adaptation

#ifndef ANY_H_
#define ANY_H_

#include <iostream>
#include <memory>
#include <typeindex>

struct Any {
    Any(void) : m_tpIndex(std::type_index(typeid(void))) { }
    Any(Any& that) : m_ptr(that.Clone()), m_tpIndex(that.m_tpIndex) { }
    Any(Any&& that) : m_ptr(std::move(that.m_ptr)), m_tpIndex(that.m_tpIndex) { }

    template <typename U,
              class = typename std::enable_if<!std::is_same<typename std::decay<U>::type, Any>::value, U>::type>
    Any(U && value) : 
        m_ptr(new Derived<typename std::decay<U>::type>(std::forward<U>(value))),
        m_tpIndex(std::type_index(typeid(typename std::decay<U>::type))) { }

    bool IsNull() const { return !bool(m_ptr); }

    template <class U> bool Is() const {
        return m_tpIndex == std::type_index(typeid(U));
    }

    // 将Any转换为实际类型
    template <class U>
    U& AnyCast() {
        if (!Is<U> ()) {
            std::cout << "can not cast " << typeid(U).name() <<
            " to " << m_tpIndex.name() << std::endl;
            throw std::bad_cast();
        }
        auto derived = dynamic_cast<Derived<U>*> (m_ptr.get());
        return derived->m_value;
    }

    Any& operator= (const Any& a) {
        if (m_ptr == nullptr) return *this;
        if (m_ptr == a.m_ptr) {
            return *this;
        }
        m_ptr = a.Clone();
        m_tpIndex = a.m_tpIndex;
        return *this;
    }

    private:
        struct Base;
        using BasePtr = std::unique_ptr<Base> ;

        struct Base {
            virtual ~Base() {}
            virtual BasePtr Clone() const = 0;
        };

        template <typename T>
        struct Derived : Base {
            template <typename U>
            Derived(U && value) : m_value(std::forward<U>(value)) { }

            BasePtr Clone() const {
                return BasePtr(new Derived<T> (m_value));
            }

            T m_value;
        };

        BasePtr Clone() const {
            if (m_ptr != nullptr) {
                return m_ptr->Clone();
            }

            return nullptr;
        }

        BasePtr m_ptr;
        std::type_index m_tpIndex;
};

#endif