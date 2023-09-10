#pragma once

#include "ttyWH.h"

#define VGA_COLOR_BLACK          ((unsigned char)0)
#define VGA_COLOR_BLUE           ((unsigned char)1)
#define VGA_COLOR_GREEN          ((unsigned char)2)
#define VGA_COLOR_CYAN           ((unsigned char)3)
#define VGA_COLOR_RED            ((unsigned char)4)
#define VGA_COLOR_MAGENTA        ((unsigned char)5)
#define VGA_COLOR_BROWN          ((unsigned char)6)
#define VGA_COLOR_LIGHT_GREY     ((unsigned char)7)
#define VGA_COLOR_DARK_GREY      ((unsigned char)8)
#define VGA_COLOR_LIGHT_BLUE     ((unsigned char)9)
#define VGA_COLOR_LIGHT_GREEN    ((unsigned char)10)
#define VGA_COLOR_LIGHT_CYAN     ((unsigned char)11)
#define VGA_COLOR_LIGHT_RED      ((unsigned char)12)
#define VGA_COLOR_LIGHT_MAGENTA  ((unsigned char)13)
#define VGA_COLOR_LIGHT_BROWN    ((unsigned char)14)
#define VGA_COLOR_WHITE          ((unsigned char)15)

void ttySetChar(unsigned char, unsigned char, char);//设置一个位置的字符
void ttySetTheme(unsigned char, unsigned char, unsigned char, unsigned char);//设置一个位置的主题
void ttyPutChar(char);//打印单个字符
void ttyPutChar(unsigned char, unsigned char, char);//打印单个字符加字符主题
void ttyPutStr(const char *);//打印字符串加主题
void ttyPutStr(unsigned char, unsigned char, const char *);//打印字符串
void ttyScrollUp();//向上滚
void ttyClear();//清屏
void ttySetTheme(unsigned char, unsigned char);//设置全局主题
void ttySetCursor(unsigned char, unsigned char);//设置光标
void ttyGetCursor(unsigned char&, unsigned char&);//获取光标
