#pragma once

class ByteArray {
public:
    ByteArray();

    ByteArray(const ByteArray &);

    ByteArray(const char *);

    ByteArray(const char *, unsigned int);

    ByteArray &operator=(const ByteArray &);

    ByteArray &operator=(const char *);

    ~ByteArray();

    unsigned int size() const;

    char &operator[](unsigned int);

    char operator[](unsigned int) const;

    char *data() const;

    operator char*() const;

    ByteArray operator+(const ByteArray &);

    ByteArray operator+(const char *);

    ByteArray &operator+=(const ByteArray &);

    ByteArray &operator+=(const char *);

    ByteArray operator+(char);

    ByteArray &operator+=(char);

private:
    void joint(const ByteArray &);

    char err;
    char *data_ = nullptr;
    unsigned int size_ = 0;

    friend bool operator==(const ByteArray &, const ByteArray &);
    friend bool operator==(const char *, const ByteArray &);
    friend bool operator==(const ByteArray &, const char *);
    friend bool operator!=(const ByteArray &, const ByteArray &);
    friend bool operator!=(const char *, const ByteArray &);
    friend bool operator!=(const ByteArray &, const char *);
};

bool operator==(const ByteArray &, const ByteArray &);
bool operator==(const char *, const ByteArray &);
bool operator==(const ByteArray &, const char *);

bool operator!=(const ByteArray &, const ByteArray &);
bool operator!=(const char *, const ByteArray &);
bool operator!=(const ByteArray &, const char *);
