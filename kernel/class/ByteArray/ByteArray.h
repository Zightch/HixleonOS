#pragma once

#include "SharedPtr.h"

class ByteArray {
public:
    ByteArray();

    ByteArray(const ByteArray &);

    ByteArray(const char *);

    ByteArray(const char *, int);

    ByteArray &operator=(const ByteArray &);

    ByteArray &operator=(const char *);

    ~ByteArray();

    int size() const;

    char &operator[](int);

    char operator[](int) const;

    char *data() const;

    operator char*() const;

    ByteArray operator+(const ByteArray &);

    ByteArray operator+(const char *);

    ByteArray &operator+=(const ByteArray &);

    ByteArray &operator+=(const char *);

    ByteArray operator+(char);

    ByteArray &operator+=(char);

    bool empty() const;

private:
    void joint(const ByteArray &);

    char err;
    char *data_ = nullptr;
    int size_ = 0;

    friend bool operator==(const ByteArray &, const ByteArray &);
    friend bool operator==(const char *, const ByteArray &);
    friend bool operator==(const ByteArray &, const char *);
    friend bool operator!=(const ByteArray &, const ByteArray &);
    friend bool operator!=(const char *, const ByteArray &);
    friend bool operator!=(const ByteArray &, const char *);
    friend ByteArray operator+(const char*, const ByteArray&);
};

bool operator==(const ByteArray &, const ByteArray &);
bool operator==(const char *, const ByteArray &);
bool operator==(const ByteArray &, const char *);

bool operator!=(const ByteArray &, const ByteArray &);
bool operator!=(const char *, const ByteArray &);
bool operator!=(const ByteArray &, const char *);

ByteArray operator+(const char *, const ByteArray &);

typedef SharedPtr<ByteArray> ByteArrayPtr;
