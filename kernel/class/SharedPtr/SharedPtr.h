#pragma once

#include "Map.hpp"

void initRefCount();//初始化引用计数器
void destroyRefCount();//销毁引用计数器

template<typename T>
class SharedPtr final {
public:
    SharedPtr();                                   //默认构造
    SharedPtr(T *);                                //有参构造
    SharedPtr(const SharedPtr &);                  //拷贝构造(浅拷贝)
    ~SharedPtr();                                  //析构函数
    SharedPtr &operator=(const SharedPtr &);       //赋值函数重载
    T *operator->() const;                         //指针运算符重载
    T &operator*() const;                          //取值运算符重载
    int useCount() const;                          //引用数量
    void unRef();                                  //解除引用
private:
    T *ptr = nullptr;//对象指针
};

typedef Map<unsigned int, unsigned int> RefCount;
extern RefCount *SharedRefCount;

template<typename T>
SharedPtr<T>::SharedPtr() {
    ptr = new T;
    (*SharedRefCount)[(unsigned int) ptr]++;
}

template<typename T>
SharedPtr<T>::SharedPtr(T *p) {
    ptr = p;
    (*SharedRefCount)[(unsigned int) ptr]++;
}

template<typename T>
SharedPtr<T>::SharedPtr(const SharedPtr &obj) {
    ptr = obj.ptr;
    (*SharedRefCount)[(unsigned int) ptr]++;
}

template<typename T>
SharedPtr<T>::~SharedPtr() {
    unRef();
}

template<typename T>
void SharedPtr<T>::unRef() {
    if (ptr == nullptr)
        return;
    if ((*SharedRefCount).contain((unsigned int) ptr)) {
        (*SharedRefCount)[(unsigned int) ptr]--;
        if ((*SharedRefCount)[(unsigned int) ptr] == 0) {
            (*SharedRefCount).remove((unsigned int) ptr);
            delete ptr;
            ptr = nullptr;
        }
    }
}

template<typename T>
SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr &obj) {
    unRef();
    ptr = obj.ptr;
    (*SharedRefCount)[(unsigned int) ptr]++;
    return *this;
}

template<typename T>
T *SharedPtr<T>::operator->() const {
    return ptr;
}

template<typename T>
T &SharedPtr<T>::operator*() const {
    return *ptr;
}

template<typename T>
int SharedPtr<T>::useCount() const {
    if ((*SharedRefCount).contain((unsigned int) ptr))
        return (*SharedRefCount)[(unsigned int) ptr];
    return 0;
}
