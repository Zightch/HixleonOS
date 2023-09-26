#pragma once

class ByteArray;

/**
 * @brief 整数转字节数组, 仅10进制支持负号('-')
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

/**
 * @brief 浮点数转字节数组
 * 
 * @param num: 数字
 * @param acc: 小数精度(小数点后几位数字)
 * @param fieldWidth: 字段宽度
 * @param fillChar: 不足宽度填充字符
 * @return ByteArray: 转换后的字节数组
 */

ByteArray toByteArray(float num, unsigned char acc = 6, unsigned char fieldWidth = 0, char fillChar = '0');
ByteArray toByteArray(double num, unsigned char acc = 6, unsigned char fieldWidth = 0, char fillChar = '0');
