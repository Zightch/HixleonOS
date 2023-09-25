#pragma once

class ByteArray;

/**
 * @brief 数字转字节数组, 仅10进制支持正负
 * 
 * @param num: 数字
 * @param base: 进制(可选: 2, 8, 10, 16)
 * @param fieldWidth: 字段宽度
 * @param fillChar: 不足宽度填充字符
 * @return ByteArray: 转换后的字节数组
 */

ByteArray toByteArray(char num, char base = 10, unsigned char fieldWidth = 0, char fillChar = '0');
ByteArray toByteArray(unsigned char num, char base = 10, unsigned char fieldWidth = 0, char fillChar = '0');
ByteArray toByteArray(short num, char base = 10, unsigned char fieldWidth = 0, char fillChar = '0');
ByteArray toByteArray(unsigned short num, char base = 10, unsigned char fieldWidth = 0, char fillChar = '0');
ByteArray toByteArray(int num, char base = 10, unsigned char fieldWidth = 0, char fillChar = '0');
ByteArray toByteArray(unsigned int num, char base = 10, unsigned char fieldWidth = 0, char fillChar = '0');
ByteArray toByteArray(long long num, char base = 10, unsigned char fieldWidth = 0, char fillChar = '0');
ByteArray toByteArray(unsigned long long num, char base = 10, unsigned char fieldWidth = 0, char fillChar = '0');
