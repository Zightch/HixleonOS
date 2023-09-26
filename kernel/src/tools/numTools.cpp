#include "tools/numTools.h"
#include "ByteArray/ByteArray.h"

//---从网上找的基于32位处理器的64位整数除法和整数取余, 用于通过编译---

unsigned long long ctz (unsigned long long value) {
    unsigned long long count = 0;
    while ((value & 1) == 0) {
        value >>= 1;
        count++;
    }
    return count;
}

extern "C" unsigned long long __umoddi3(unsigned long long a, unsigned long long b) {
    // 如果b为0, 返回a
    if (b == 0) return a;
    // 如果a和b都小于2^32, 直接用32位运算
    if ((a >> 32) == 0 && (b >> 32) == 0) return (unsigned int) a % (unsigned int) b;
    // 如果a小于b, 返回a
    if (a < b) return a;
    // 如果b为2的幂, 用位运算
    if ((b & (b - 1)) == 0) return a & (b - 1);
    // 其他情况, 用循环减法
    unsigned long long r = a;
    while (r >= b) r -= b;
    return r;
}

extern "C" unsigned long long __udivdi3(unsigned long long a, unsigned long long b) {
    // 如果b为0, 返回a
    if (b == 0) return a;
    // 如果a和b都小于2^32, 直接用32位运算
    if ((a >> 32) == 0 && (b >> 32) == 0) return (unsigned int) a / (unsigned int) b;
    // 如果a小于b, 返回0
    if (a < b) return 0;
    // 如果b为2的幂, 用位运算
    if ((b & (b - 1)) == 0) return a >> ctz(b);
    // 其他情况, 用循环减法
    unsigned long long q = 0; // 商
    unsigned long long r = a; // 被除数
    while (r >= b) {
        r -= b;
        q++;
    }
    return q;
}

extern "C" long long __divdi3(long long a, long long b) {
    // 如果b为0, 返回a
    if (b == 0) return a;
    // 如果a和b都小于2^32, 直接用32位运算
    if ((a >> 32) == 0 && (b >> 32) == 0) return (int) a / (int) b;
    // 如果a和b同号, 用__udivdi3计算绝对值的商
    if ((a > 0 && b > 0) || (a < 0 && b < 0))
        return __udivdi3(a < 0 ? -a : a, b < 0 ? -b : b);
        // 如果a和b异号, 用__udivdi3计算绝对值的商, 并取相反数
    else return -__udivdi3(a < 0 ? -a : a, b < 0 ? -b : b);
}

extern "C" long long __moddi3(long long a, long long b) {
    // 如果b为0, 返回a
    if (b == 0) return a;
    // 如果a和b都小于2^32, 直接用32位运算
    if ((a >> 32) == 0 && (b >> 32) == 0) return (int) a % (int) b;
    // 如果a和b同号, 用__umoddi3计算绝对值的余数
    if ((a > 0 && b > 0) || (a < 0 && b < 0))
        return __umoddi3(a < 0 ? -a : a, b < 0 ? -b : b);
        // 如果a和b异号, 用__umoddi3计算绝对值的余数, 并取相反数
    else return -__umoddi3(a < 0 ? -a : a, b < 0 ? -b : b);
}

ByteArray toByteArray(char num, char base, unsigned char fieldWidth, char fillChar) {
    ByteArray ba;
    switch (base) {
        case 2: {
            bool start = false;
            // 一个char有8位, 从最高位开始判断
            for (int i = 7; i >= 0; i--) {
                // 右移i位, 与1进行与运算, 得到第i位的值
                char bit = (num >> i) & 1;
                if (bit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (bit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 8: {
            bool start = false;
            char digit = (char) ((num >> 6) & 0x3);
            if (digit != 0 && !start) start = true;
            if (start) ba += (char) (digit + 48);
            for (int i = 3; i >= 0; i -= 3) {
                // 右移i位, 与7进行与运算, 得到第i/3+1位的值
                digit = (num >> i) & 7;
                if (digit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (digit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 10: {
            char numAbs = num < 0 ? -num : num;
            ByteArray tmp;
            while (numAbs != 0) {
                tmp += (char) ((numAbs % 10) + 48);
                numAbs /= 10;
            }
            // 反转tmp
            for (int i = tmp.size() - 1; i >= 0; i--)
                ba += tmp[i];
            if (ba.empty())ba += '0';
            break;
        }
        case 16: {
            bool start = false;
            // 一个char有8位, 每4位对应一位十六进制数, 从最高位开始判断
            for (int i = 4; i >= 0; i -= 4) {
                // 右移i位, 与15进行与运算, 得到第i/4+1位的值
                char digit = (num >> i) & 15;
                if (digit != 0 && !start) start = true;
                if (start) {
                    if (digit < 10)
                        ba += (char) (digit + 48);
                    else
                        ba += (char) (digit + 55); // A~F
                }
            }
            if (ba.empty())ba += '0';
            break;
        }
        default:
            break;
    }
    if (!ba.empty()) {
        if (ba.size() < fieldWidth) {
            ByteArray fill;
            for (int i = 0; i < fieldWidth - ba.size(); i++)
                fill += fillChar;
            if (base == 10 && num < 0)fill[0] = '-';
            ba = fill + ba;
        } else if (base == 10 && num < 0)ba = "-" + ba;
    }
    return ba;
}
ByteArray toByteArray(unsigned char num, char base, unsigned char fieldWidth, char fillChar) {
    ByteArray ba;
    switch (base) {
        case 2: {
            bool start = false;
            // 一个char有8位, 从最高位开始判断
            for (int i = 7; i >= 0; i--) {
                // 右移i位, 与1进行与运算, 得到第i位的值
                char bit = (num >> i) & 1;
                if (bit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (bit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 8: {
            bool start = false;
            char digit = (char) ((num >> 6) & 0x3);
            if (digit != 0) start = true;
            if (start) ba += (char) (digit + 48);
            for (int i = 3; i >= 0; i -= 3) {
                // 右移i位, 与7进行与运算, 得到第i/3+1位的值
                digit = (num >> i) & 7;
                if (digit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (digit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 10: {
            ByteArray tmp;
            while (num != 0) {
                tmp += (char) ((num % 10) + 48);
                num /= 10;
            }
            // 反转tmp
            for (int i = tmp.size() - 1; i >= 0; i--)
                ba += tmp[i];
            if (ba.empty())ba += '0';
            break;
        }
        case 16: {
            bool start = false;
            // 一个char有8位, 每4位对应一位十六进制数, 从最高位开始判断
            for (int i = 4; i >= 0; i -= 4) {
                // 右移i位, 与15进行与运算, 得到第i/4+1位的值
                char digit = (num >> i) & 15;
                if (digit != 0 && !start) start = true;
                if (start) {
                    if (digit < 10)
                        ba += (char) (digit + 48);
                    else
                        ba += (char) (digit + 55); // A~F
                }
            }
            if (ba.empty())ba += '0';
            break;
        }
        default:
            break;
    }
    if (!ba.empty())
        if (ba.size() < fieldWidth) {
            ByteArray fill;
            for (int i = 0; i < fieldWidth - ba.size(); i++)
                fill += fillChar;
            ba = fill + ba;
        }
    return ba;
}
ByteArray toByteArray(short num, char base, unsigned char fieldWidth, char fillChar) {
    ByteArray ba;
    switch (base) {
        case 2: {
            bool start = false;
            // 16位, 从最高位开始判断
            for (int i = 15; i >= 0; i--) {
                // 右移i位, 与1进行与运算, 得到第i位的值
                char bit = (num >> i) & 1;
                if (bit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (bit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 8: {
            bool start = false;
            char digit = (char) ((num >> 15) & 0x1);
            if (digit != 0) start = true;
            if (start) ba += (char) (digit + 48);
            for (int i = 12; i >= 0; i -= 3) {
                // 右移i位, 与7进行与运算, 得到第i/3+1位的值
                digit = (num >> i) & 7;
                if (digit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (digit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 10: {
            short numAbs = num < 0 ? -num : num;
            ByteArray tmp;
            while (numAbs != 0) {
                tmp += (char) ((numAbs % 10) + 48);
                numAbs /= 10;
            }
            // 反转tmp
            for (int i = tmp.size() - 1; i >= 0; i--)
                ba += tmp[i];
            if (ba.empty())ba += '0';
            break;
        }
        case 16: {
            bool start = false;
            // 16位, 每4位对应一位十六进制数, 从最高位开始判断
            for (int i = 12; i >= 0; i -= 4) {
                // 右移i位, 与15进行与运算, 得到第i/4+1位的值
                char digit = (num >> i) & 15;
                if (digit != 0 && !start) start = true;
                if (start) {
                    if (digit < 10)
                        ba += (char) (digit + 48);
                    else
                        ba += (char) (digit + 55); // A~F
                }
            }
            if (ba.empty())ba += '0';
            break;
        }
        default:
            break;
    }
    if (!ba.empty()) {
        if (ba.size() < fieldWidth) {
            ByteArray fill;
            for (int i = 0; i < fieldWidth - ba.size(); i++)
                fill += fillChar;
            if (base == 10 && num < 0)fill[0] = '-';
            ba = fill + ba;
        } else if (base == 10 && num < 0)ba = "-" + ba;
    }
    return ba;
}
ByteArray toByteArray(unsigned short num, char base, unsigned char fieldWidth, char fillChar) {
    ByteArray ba;
    switch (base) {
        case 2: {
            bool start = false;
            // 16位, 从最高位开始判断
            for (int i = 15; i >= 0; i--) {
                // 右移i位, 与1进行与运算, 得到第i位的值
                char bit = (num >> i) & 1;
                if (bit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (bit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 8: {
            bool start = false;
            char digit = (char) ((num >> 15) & 0x1);
            if (digit != 0) start = true;
            if (start) ba += (char) (digit + 48);
            for (int i = 12; i >= 0; i -= 3) {
                // 右移i位, 与7进行与运算, 得到第i/3+1位的值
                digit = (num >> i) & 7;
                if (digit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (digit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 10: {
            ByteArray tmp;
            while (num != 0) {
                tmp += (char) ((num % 10) + 48);
                num /= 10;
            }
            // 反转tmp
            for (int i = tmp.size() - 1; i >= 0; i--)
                ba += tmp[i];
            if (ba.empty())ba += '0';
            break;
        }
        case 16: {
            bool start = false;
            // 16位, 每4位对应一位十六进制数, 从最高位开始判断
            for (int i = 12; i >= 0; i -= 4) {
                // 右移i位, 与15进行与运算, 得到第i/4+1位的值
                char digit = (num >> i) & 15;
                if (digit != 0 && !start) start = true;
                if (start) {
                    if (digit < 10)
                        ba += (char) (digit + 48);
                    else
                        ba += (char) (digit + 55); // A~F
                }
            }
            if (ba.empty())ba += '0';
            break;
        }
        default:
            break;
    }
    if (!ba.empty())
        if (ba.size() < fieldWidth) {
            ByteArray fill;
            for (int i = 0; i < fieldWidth - ba.size(); i++)
                fill += fillChar;
            ba = fill + ba;
        }
    return ba;
}
ByteArray toByteArray(int num, char base, unsigned char fieldWidth, char fillChar) {
    ByteArray ba;
    switch (base) {
        case 2: {
            bool start = false;
            // 32位, 从最高位开始判断
            for (int i = 31; i >= 0; i--) {
                // 右移i位, 与1进行与运算, 得到第i位的值
                char bit = (num >> i) & 1;
                if (bit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (bit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 8: {
            bool start = false;
            char digit = (char) ((num >> 30) & 0x3);
            if (digit != 0) start = true;
            if (start) ba += (char) (digit + 48);
            for (int i = 27; i >= 0; i -= 3) {
                // 右移i位, 与7进行与运算, 得到第i/3+1位的值
                digit = (num >> i) & 7;
                if (digit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (digit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 10: {
            int numAbs = num < 0 ? -num : num;
            ByteArray tmp;
            while (numAbs != 0) {
                tmp += (char) ((numAbs % 10) + 48);
                numAbs /= 10;
            }
            // 反转tmp
            for (int i = tmp.size() - 1; i >= 0; i--)
                ba += tmp[i];
            if (ba.empty())ba += '0';
            break;
        }
        case 16: {
            bool start = false;
            // 32位, 每4位对应一位十六进制数, 从最高位开始判断
            for (int i = 28; i >= 0; i -= 4) {
                // 右移i位, 与15进行与运算, 得到第i/4+1位的值
                char digit = (num >> i) & 15;
                if (digit != 0 && !start) start = true;
                if (start) {
                    if (digit < 10)
                        ba += (char) (digit + 48);
                    else
                        ba += (char) (digit + 55); // A~F
                }
            }
            if (ba.empty())ba += '0';
            break;
        }
        default:
            break;
    }
    if (!ba.empty()) {
        if (ba.size() < fieldWidth) {
            ByteArray fill;
            for (int i = 0; i < fieldWidth - ba.size(); i++)
                fill += fillChar;
            if (base == 10 && num < 0)fill[0] = '-';
            ba = fill + ba;
        } else if (base == 10 && num < 0)ba = "-" + ba;
    }
    return ba;
}
ByteArray toByteArray(unsigned int num, char base, unsigned char fieldWidth, char fillChar) {
    ByteArray ba;
    switch (base) {
        case 2: {
            bool start = false;
            // 32位, 从最高位开始判断
            for (int i = 31; i >= 0; i--) {
                // 右移i位, 与1进行与运算, 得到第i位的值
                char bit = (num >> i) & 1;
                if (bit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (bit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 8: {
            bool start = false;
            char digit = (char) ((num >> 30) & 0x3);
            if (digit != 0) start = true;
            if (start) ba += (char) (digit + 48);
            for (int i = 27; i >= 0; i -= 3) {
                // 右移i位, 与7进行与运算, 得到第i/3+1位的值
                digit = (num >> i) & 7;
                if (digit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (digit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 10: {
            ByteArray tmp;
            while (num != 0) {
                tmp += (char) ((num % 10) + 48);
                num /= 10;
            }
            // 反转tmp
            for (int i = tmp.size() - 1; i >= 0; i--)
                ba += tmp[i];
            if (ba.empty())ba += '0';
            break;
        }
        case 16: {
            bool start = false;
            // 32位, 每4位对应一位十六进制数, 从最高位开始判断
            for (int i = 28; i >= 0; i -= 4) {
                // 右移i位, 与15进行与运算, 得到第i/4+1位的值
                char digit = (num >> i) & 15;
                if (digit != 0 && !start) start = true;
                if (start) {
                    if (digit < 10)
                        ba += (char) (digit + 48);
                    else
                        ba += (char) (digit + 55); // A~F
                }
            }
            if (ba.empty())ba += '0';
            break;
        }
        default:
            break;
    }
    if (!ba.empty())
        if (ba.size() < fieldWidth) {
            ByteArray fill;
            for (int i = 0; i < fieldWidth - ba.size(); i++)
                fill += fillChar;
            ba = fill + ba;
        }
    return ba;
}
ByteArray toByteArray(long long num, char base, unsigned char fieldWidth, char fillChar) {
    ByteArray ba;
    switch (base) {
        case 2: {
            bool start = false;
            // 64位, 从最高位开始判断
            for (int i = 63; i >= 0; i--) {
                // 右移i位, 与1进行与运算, 得到第i位的值
                char bit = (num >> i) & 1;
                if (bit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (bit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 8: {
            bool start = false;
            char digit = (char) ((num >> 63) & 0x1);
            if (digit != 0) start = true;
            if (start) ba += (char) (digit + 48);
            for (int i = 60; i >= 0; i -= 3) {
                // 右移i位, 与7进行与运算, 得到第i/3+1位的值
                digit = (num >> i) & 7;
                if (digit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (digit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 10: {
            long long numAbs = num < 0 ? -num : num;
            ByteArray tmp;
            while (numAbs != 0) {
                tmp += (char) ((numAbs % 10) + 48);
                numAbs /= 10;
            }
            // 反转tmp
            for (int i = tmp.size() - 1; i >= 0; i--)
                ba += tmp[i];
            if (ba.empty())ba += '0';
            break;
        }
        case 16: {
            bool start = false;
            // 64位, 每4位对应一位十六进制数, 从最高位开始判断
            for (int i = 60; i >= 0; i -= 4) {
                // 右移i位, 与15进行与运算, 得到第i/4+1位的值
                char digit = (num >> i) & 15;
                if (digit != 0 && !start) start = true;
                if (start) {
                    if (digit < 10)
                        ba += (char) (digit + 48);
                    else
                        ba += (char) (digit + 55); // A~F
                }
            }
            if (ba.empty())ba += '0';
            break;
        }
        default:
            break;
    }
    if (!ba.empty()) {
        if (ba.size() < fieldWidth) {
            ByteArray fill;
            for (int i = 0; i < fieldWidth - ba.size(); i++)
                fill += fillChar;
            if (base == 10 && num < 0)fill[0] = '-';
            ba = fill + ba;
        } else if (base == 10 && num < 0)ba = "-" + ba;
    }
    return ba;
}
ByteArray toByteArray(unsigned long long num, char base, unsigned char fieldWidth, char fillChar) {
    ByteArray ba;
    switch (base) {
        case 2: {
            bool start = false;
            // 64位, 从最高位开始判断
            for (int i = 63; i >= 0; i--) {
                // 右移i位, 与1进行与运算, 得到第i位的值
                char bit = (num >> i) & 1;
                if (bit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (bit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 8: {
            bool start = false;
            char digit = (char) ((num >> 63) & 0x1);
            if (digit != 0) start = true;
            if (start) ba += (char) (digit + 48);
            for (int i = 60; i >= 0; i -= 3) {
                // 右移i位, 与7进行与运算, 得到第i/3+1位的值
                digit = (num >> i) & 7;
                if (digit != 0 && !start) start = true;
                // 转换为字符并添加到字节数组
                if (start) ba += (char) (digit + 48);
            }
            if (ba.empty())ba += '0';
            break;
        }
        case 10: {
            ByteArray tmp;
            while (num != 0) {
                tmp += (char) ((num % 10) + 48);
                num /= 10;
            }
            // 反转tmp
            for (int i = tmp.size() - 1; i >= 0; i--)
                ba += tmp[i];
            if (ba.empty())ba += '0';
            break;
        }
        case 16: {
            bool start = false;
            // 64位, 每4位对应一位十六进制数, 从最高位开始判断
            for (int i = 60; i >= 0; i -= 4) {
                // 右移i位, 与15进行与运算, 得到第i/4+1位的值
                char digit = (num >> i) & 15;
                if (digit != 0 && !start) start = true;
                if (start) {
                    if (digit < 10)
                        ba += (char) (digit + 48);
                    else
                        ba += (char) (digit + 55); // A~F
                }
            }
            if (ba.empty())ba += '0';
            break;
        }
        default:
            break;
    }
    if (!ba.empty())
        if (ba.size() < fieldWidth) {
            ByteArray fill;
            for (int i = 0; i < fieldWidth - ba.size(); i++)
                fill += fillChar;
            ba = fill + ba;
        }
    return ba;
}

ByteArray toByteArray(float num, unsigned char acc, unsigned char fieldWidth, char fillChar) {
    return toByteArray((double) num, acc, fieldWidth, fillChar);
}

ByteArray toByteArray(double num, unsigned char acc, unsigned char fieldWidth, char fillChar) {
    double numAbs = num < 0 ? -num : num;
    long long integer = numAbs;
    double decimals = numAbs - (integer * 1.0);
    long long decCarry = 0;//小数进位
    for (short i = 0; i < acc; i++) {
        decimals *= 10;
        decCarry *= 10;
        decCarry += 9;
    }
    {//四舍五入
        long long dec = decimals;
        double decOfDec = decimals - (dec * 1.0);
        if (decOfDec >= 0.5)decimals += 1.0;
        if (decimals > decCarry) {//如果超过小数进位
            decimals -= (decCarry + 1);
            integer++;
        }
    }
    ByteArray ba = toByteArray(integer) + "." + toByteArray((long long) decimals, 10, acc);
    if (ba.size() < fieldWidth) {
        ByteArray fill;
        for (int i = 0; i < fieldWidth - ba.size(); i++)
            fill += fillChar;
        if (num < 0)fill[0] = '-';
        ba = fill + ba;
    } else if (num < 0)ba = "-" + ba;
    return ba;
}
