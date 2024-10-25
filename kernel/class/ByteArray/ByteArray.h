#pragma once

#include "SharedPtr/SharedPtr.h"

class ByteArray final {
public:
    ByteArray();

    ByteArray(const ByteArray &);

    explicit ByteArray(const char *);

    ByteArray(const char *, int);

    ByteArray &operator=(const ByteArray &);

    ByteArray &operator=(const char *);

    ~ByteArray();

    int size() const;

    char &operator[](int);

    char operator[](int) const;

    char *data() const;

    explicit operator char*() const;

    ByteArray operator+(const ByteArray &) const;

    ByteArray operator+(const char *) const;

    ByteArray &operator+=(const ByteArray &);

    ByteArray &operator+=(const char *);

    ByteArray operator+(char) const;

    ByteArray &operator+=(char);

    bool empty() const;

private:
    void joint(const ByteArray &);

    char err = 0;
    char *content = nullptr;
    int length = 0;

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
