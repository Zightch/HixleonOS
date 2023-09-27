#pragma once

template<typename T>
class SharedPtr final {
public:
    SharedPtr();                                   //默认构造
    SharedPtr(T *);                                //有参构造
    SharedPtr(const SharedPtr &);               //拷贝构造(浅拷贝)
    ~SharedPtr();                                  //析构函数
    SharedPtr &operator=(const SharedPtr &);    //赋值函数重载
    T *operator->() const;                         //指针运算符重载
    T &operator*() const;                          //取值运算符重载
    int useCount() const;                          //引用数量
    void unRef();                                  //解除引用
private:
    T *ptr = nullptr;//对象指针
    int *ptrCount = nullptr;//引用计数器
};

template<typename T>
SharedPtr<T>::SharedPtr() {
    ptr = new T;
    ptrCount = new int;
    *ptrCount = 1;
}

template<typename T>
SharedPtr<T>::SharedPtr(T *p) {
    ptr = p;
    ptrCount = new int;
    *ptrCount = 1;
}

template<typename T>
SharedPtr<T>::SharedPtr(const SharedPtr &obj) {
    ptr = obj.ptr;
    ptrCount = obj.ptrCount;
    (*ptrCount)++;
}

template<typename T>
SharedPtr<T>::~SharedPtr() {
    unRef();
}

template<typename T>
void SharedPtr<T>::unRef() {
    if (ptrCount != nullptr) {
        (*ptrCount)--;
        if (*ptrCount == 0) {
            delete ptr;
            ptr = nullptr;
            delete ptrCount;
            ptrCount = nullptr;
        }
    }
}

template<typename T>
SharedPtr<T> &SharedPtr<T>::operator=(const SharedPtr &obj) {
    unRef();
    ptr = obj.ptr;
    ptrCount = obj.ptrCount;
    (*ptrCount)++;
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
    if (ptrCount != nullptr)
        return *ptrCount;
    return 0;
}
