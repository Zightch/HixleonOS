#include "ByteArray.h"

ByteArray::ByteArray() {}

ByteArray::ByteArray(const ByteArray &ba) {
    data_ = new char[ba.size_];
    for (unsigned int i = 0; i < ba.size_; i++)
        data_[i] = ba.data_[i];
    size_ = ba.size_;
}

ByteArray::ByteArray(const char *c) {
    if (c == nullptr)
        return;
    while (c[size_++]);
    size_--;
    data_ = new char[size_];
    for (unsigned int i = 0; i < size_; i++)
        data_[i] = c[i];
}

ByteArray::ByteArray(const char *c, unsigned int size) {
    if (c == nullptr || size == 0)
        return;
    size_ = size;
    data_ = new char[size_];
    for (unsigned int i = 0; i < size_; i++)
        data_[i] = c[i];
}

ByteArray &ByteArray::operator=(const ByteArray &ba) {
    delete[]data_;
    data_ = new char[ba.size_];
    for (unsigned int i = 0; i < ba.size_; i++)
        data_[i] = ba.data_[i];
    size_ = ba.size_;
    return *this;
}

ByteArray &ByteArray::operator=(const char *c) {
    delete[]data_;
    data_ = nullptr;
    size_ = 0;
    if (c == nullptr)
        return *this;
    while (c[size_++]);
    size_--;
    data_ = new char[size_];
    for (unsigned int i = 0; i < size_; i++)
        data_[i] = c[i];
    return *this;
}

ByteArray::~ByteArray() {
    delete[]data_;
    data_ = nullptr;
    size_ = 0;
}

unsigned int ByteArray::size() const {
    return size_;
}

char &ByteArray::operator[](unsigned int size) {
    if (size < size_)
        return data_[size];
    return err;
}

char ByteArray::operator[](unsigned int size) const {
    if (size < size_)
        return data_[size];
    return 0;
}

void ByteArray::joint(const ByteArray &ba) {
    char *tmp = data_;
    size_ += ba.size_;
    data_ = new char[size_];
    for (unsigned long long i = 0; i < size_ - ba.size_; i++)
        data_[i] = tmp[i];
    delete[]tmp;
    for (unsigned long long i = size_ - ba.size_, j = 0; i < size_; i++, j++)
        data_[i] = ba.data_[j];
}

char *ByteArray::data() const {
    return data_;
}

ByteArray::operator char*() const {
    return data_;
}

ByteArray ByteArray::operator+(const ByteArray &ba) {
    ByteArray tmp = *this;
    tmp.joint(ba);
    return tmp;
}

ByteArray ByteArray::operator+(const char *c) {
    ByteArray tmp = *this;
    tmp.joint(c);
    return tmp;
}

ByteArray &ByteArray::operator+=(const ByteArray &ba) {
    this->joint(ba);
    return *this;
}

ByteArray &ByteArray::operator+=(const char *c) {
    this->joint(c);
    return *this;
}

ByteArray ByteArray::operator+(char c) {
    ByteArray tmp0 = *this;
    ByteArray tmp1;
    tmp1.data_ = new char[1];
    tmp1.data_[0] = c;
    tmp1.size_ = 1;
    tmp0.joint(tmp1);
    return tmp0;
}

ByteArray &ByteArray::operator+=(char c) {
    ByteArray tmp;
    tmp.data_ = new char[1];
    tmp.data_[0] = c;
    tmp.size_ = 1;
    this->joint(tmp);
    return *this;
}

bool operator==(const ByteArray &b1, const ByteArray &b2) {
    if (b1.size_ != b2.size_)
        return false;
    for (unsigned int i = 0; i < b1.size(); i++)
        if (b1.data_[i] != b2.data_[i])return false;
    return true;
}

bool operator==(const char *c, const ByteArray &b2) {
    ByteArray b1 = c;
    return b1 == b2;
}

bool operator==(const ByteArray &b1, const char *c) {
    ByteArray b2 = c;
    return b1 == b2;
}

bool operator!=(const ByteArray &b1, const ByteArray &b2) {
    return !(b1 == b2);
}
bool operator!=(const char *c, const ByteArray &b2) {
    return !(c == b2);
}
bool operator!=(const ByteArray &b1, const char *c) {
    return !(b1 == c);
}
