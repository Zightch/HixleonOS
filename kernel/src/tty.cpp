#include "tty.h"
#include "cpu.h"

unsigned char *ttyBase = (unsigned char*)0xB8000;
unsigned char ttyTheme = 0x07;
unsigned short ttyOffset = 0;

//设置一个位置的字符
void ttySetChar(unsigned char x, unsigned char y, char c) {
    unsigned char tmpX, tmpY;
    ttyGetCursor(tmpX, tmpY);
    ttySetCursor(x, y);
    ttyBase[2 * ttyOffset] = c;
    ttySetCursor(tmpX, tmpY);
}

//设置一个位置的主题
void ttySetTheme(unsigned char x, unsigned char y, unsigned char fg, unsigned char bg) {
    unsigned char tmpX, tmpY;
    ttyGetCursor(tmpX, tmpY);
    ttySetCursor(x, y);
    char theme = 0x07;
    if (fg <= 15 && bg <= 15)
        theme = (char)(((bg << 4) & 0xF0) | (fg & 0x0F));
    ttyBase[2 * ttyOffset + 1] = theme;
    ttySetCursor(tmpX, tmpY);
}

//打印字符串加主题
void ttyPutStr(unsigned char fg, unsigned char bg, const char *str) {
    while (str[0] != '\0') {
        ttyPutChar(fg, bg, str[0]);
        str++;
    }
}

//打印单个字符加字符主题
void ttyPutChar(unsigned char fg, unsigned char bg, char c) {
    char theme = 0x07;
    if (fg <= 15 && bg <= 15)
        theme = (char)(((bg << 4) & 0xF0) | (fg & 0x0F));
    if ((c < 32 || c > 126) && c != '\n')c = '?';
    if (c == '\n') {
        ttyOffset /= TTY_WIDTH;
        ttyOffset += 1;
        ttyOffset *= TTY_WIDTH;
    } else {
        ttyBase[2 * ttyOffset] = c;
        ttyBase[2 * ttyOffset + 1] = theme;
        ttyOffset++;
    }
    if (ttyOffset >= 1999) {
        ttyScrollUp();
        ttyOffset -= TTY_WIDTH;
    }
    unsigned char x, y;
    ttyGetCursor(x, y);
    ttySetCursor(x, y);
}

//打印单个字符
void ttyPutChar(char ch) {
    if ((ch < 32 || ch > 126) && ch != '\n')ch = '?';
    if (ch == '\n') {
        ttyOffset /= TTY_WIDTH;
        ttyOffset += 1;
        ttyOffset *= TTY_WIDTH;
    } else {
        ttyBase[2 * ttyOffset] = ch;
        ttyOffset++;
    }
    if (ttyOffset >= 1999) {
        ttyScrollUp();
        ttyOffset -= TTY_WIDTH;
    }
    unsigned char x, y;
    ttyGetCursor(x, y);
    ttySetCursor(x, y);
}

//打印字符串
void ttyPutStr(const char *str) {
    while (str[0] != '\0') {
        ttyPutChar(str[0]);
        str++;
    }
}

//向上滚
void ttyScrollUp() {
    for (unsigned short i = TTY_WIDTH; i < TTY_WIDTH * TTY_HEIGHT; i++) {
        ttyBase[2 * (i - TTY_WIDTH)] = ttyBase[2 * i];
        ttyBase[2 * (i - TTY_WIDTH) + 1] = ttyBase[2 * i + 1];
    }
    for (unsigned short i = (TTY_HEIGHT - 1) * TTY_WIDTH; i < TTY_WIDTH * TTY_HEIGHT; i++) {
        ttyBase[2 * i] = 0;
        ttyBase[2 * i + 1] = ttyTheme;
    }
    unsigned char x, y;
    ttyGetCursor(x, y);
    if (y > 0)
        y--;
    else
        x = 0;
    ttySetCursor(x, y);
}

//清屏
void ttyClear() {
    for (unsigned short i = 0; i < TTY_HEIGHT * TTY_WIDTH; i++)
        ttyBase[2 * i] = '\0';
    ttySetCursor(0, 0);
}

//设置全局主题
void ttySetTheme(unsigned char fg, unsigned char bg) {
    char theme = 0x07;
    if (fg <= 15 && bg <= 15)
        theme = (char)(((bg << 4) & 0xF0) | (fg & 0x0F));
    for (unsigned short i = 0; i < TTY_WIDTH * TTY_HEIGHT; i++)
        ttyBase[2 * i + 1] = theme;
    ttyTheme = theme;
}

//获取光标
void ttyGetCursor(unsigned char &x, unsigned char &y) {
    x = ttyOffset % TTY_WIDTH;
    y = ttyOffset / TTY_WIDTH;
}

void ttySetCursor(unsigned char x, unsigned char y) {
    unsigned short offsetTmp = 0;
    offsetTmp = y * TTY_WIDTH + x;
    if (offsetTmp >= TTY_HEIGHT * TTY_WIDTH)
        return;

    CPU::outb(0x03d4, 0x0f);//设置为光标低8位
    CPU::outb(0x03d5, offsetTmp);

    CPU::outb(0x03d4, 0x0e);//设置为光标高8位
    CPU::outb(0x03d5, offsetTmp >> 8);

    ttyOffset = offsetTmp;
}
