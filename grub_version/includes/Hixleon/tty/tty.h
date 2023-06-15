#pragma once

#define VGA_COLOR_BLACK          ((char)0)
#define VGA_COLOR_BLUE           ((char)1)
#define VGA_COLOR_GREEN          ((char)2)
#define VGA_COLOR_CYAN           ((char)3)
#define VGA_COLOR_RED            ((char)4)
#define VGA_COLOR_MAGENTA        ((char)5)
#define VGA_COLOR_BROWN          ((char)6)
#define VGA_COLOR_LIGHT_GREY     ((char)7)
#define VGA_COLOR_DARK_GREY      ((char)8)
#define VGA_COLOR_LIGHT_BLUE     ((char)9)
#define VGA_COLOR_LIGHT_GREEN    ((char)10)
#define VGA_COLOR_LIGHT_CYAN     ((char)11)
#define VGA_COLOR_LIGHT_RED      ((char)12)
#define VGA_COLOR_LIGHT_MAGENTA  ((char)13)
#define VGA_COLOR_LIGHT_BROWN    ((char)14)
#define VGA_COLOR_WHITE          ((char)15)

void ttyPutChar(char);//打印单个字符
void ttyPutStr(char*);//打印字符串
void ttyScrollUp();//向上滚
void ttyClear();//清屏
void ttyTheme(char, char);//设置主题
