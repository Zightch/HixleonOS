#include "ByteArray.h"

ByteArray::ByteArray() = default;

ByteArray::ByteArray(const ByteArray &ba) {
    if (ba.length == 0 || ba.content == nullptr)return;
    content = new char[ba.length];
    for (int i = 0; i < ba.length; i++)
        content[i] = ba.content[i];
    length = ba.length;
}

ByteArray::ByteArray(const char *c) {
    if (c == nullptr)
        return;
    while (c[length] != 0)length++;
    content = new char[length];
    for (int i = 0; i < length; i++)
        content[i] = c[i];
}

ByteArray::ByteArray(const char *c, int size) {
    if (c == nullptr || size <= 0)
        return;
    length = size;
    content = new char[length];
    for (int i = 0; i < length; i++)
        content[i] = c[i];
}

ByteArray &ByteArray::operator=(const ByteArray &ba) {
    if (this == &ba)return *this;
    delete[]content;
    content = new char[ba.length];
    for (int i = 0; i < ba.length; i++)
        content[i] = ba.content[i];
    length = ba.length;
    return *this;
}

ByteArray &ByteArray::operator=(const char *c) {
    delete[]content;
    content = nullptr;
    length = 0;
    if (c == nullptr)
        return *this;
    while (c[length] != 0)length++;
    content = new char[length];
    for (int i = 0; i < length; i++)
        content[i] = c[i];
    return *this;
}

ByteArray::~ByteArray() {
    delete[]content;
    content = nullptr;
    length = 0;
}

int ByteArray::size() const {
    return length;
}

char &ByteArray::operator[](int index) {
    if (0 <= index && index < length)
        return content[index];
    return err;
}

char ByteArray::operator[](int index) const {
    if (0 <= index && index < length)
        return content[index];
    return 0;
}

void ByteArray::joint(const ByteArray &ba)
{
    if (ba.length == 0 || ba.content == nullptr)return;
    const char *tmp = content;
    length += ba.length;
    content = new char[length];
    if (tmp != nullptr)
    {
        for (int i = 0; i < length - ba.length; i++)
            content[i] = tmp[i];
        delete[]tmp;
    }
    for (int i = length - ba.length, j = 0; i < length; i++, j++)
        content[i] = ba.content[j];
}

char *ByteArray::data() const {
    return content;
}

ByteArray::operator char*() const {
    return content;
}

ByteArray ByteArray::operator+(const ByteArray &ba) const {
    ByteArray tmp = *this;
    tmp.joint(ba);
    return tmp;
}

ByteArray ByteArray::operator+(const char *c) const {
    ByteArray tmp = *this;
    tmp.joint(ByteArray(c));
    return tmp;
}

ByteArray &ByteArray::operator+=(const ByteArray &ba) {
    this->joint(ba);
    return *this;
}

ByteArray &ByteArray::operator+=(const char *c) {
    this->joint(ByteArray(c));
    return *this;
}

ByteArray ByteArray::operator+(char c) const {
    ByteArray tmp0 = *this;
    ByteArray tmp1;
    tmp1.content = new char[1];
    tmp1.content[0] = c;
    tmp1.length = 1;
    tmp0.joint(tmp1);
    return tmp0;
}

ByteArray &ByteArray::operator+=(char c) {
    ByteArray tmp;
    tmp.content = new char[1];
    tmp.content[0] = c;
    tmp.length = 1;
    this->joint(tmp);
    return *this;
}

bool ByteArray::empty() const {
    return length == 0;
}

bool operator==(const ByteArray &b1, const ByteArray &b2) {
    if (&b1 == &b2)return true;
    if (b1.length != b2.length)
        return false;
    for (int i = 0; i < b1.size(); i++)
        if (b1.content[i] != b2.content[i])return false;
    return true;
}

bool operator==(const char *c, const ByteArray &b2) {
    ByteArray b1 = ByteArray(c);
    return b1 == b2;
}

bool operator==(const ByteArray &b1, const char *c) {
    ByteArray b2 = ByteArray(c);
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

ByteArray operator+(const char *c, const ByteArray &ba) {
    ByteArray tmp(c);
    tmp.joint(ba);
    return tmp;
}
