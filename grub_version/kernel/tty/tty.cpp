#include "includes/Hixleon/tty/tty.h"

#define TTY_WIDTH 80
#define TTY_HEIGHT 25

char *buff = (char*)0xB8000;
char theme = 0x07;

unsigned short offset = 0;

//打印单个字符
void ttyPutChar(char ch) {
    if (ch == '\n') {
        offset /= TTY_WIDTH;
        offset += 1;
        offset *= TTY_WIDTH;
    } else if (ch == '\r') {
        offset /= TTY_WIDTH;
        offset *= TTY_WIDTH;
    } else {
        (buff + (2 * offset))[0] = ch;
        (buff + (2 * offset))[1] = theme;
        offset++;
    }
    if (offset >= 1999) {
        ttyScrollUp();
        offset -= TTY_WIDTH;
    }
}

//打印字符串
void ttyPutStr(char *str) {
    while (str[0] != '\0') {
        ttyPutChar(str[0]);
        str++;
    }
}

//向上滚
void ttyScrollUp() {
    // TODO memcpy memmov
}

//清屏
void ttyClear() {
    for (unsigned short i = 0; i < TTY_HEIGHT * TTY_WIDTH; i++) {
        buff[2 * i] = '\0';
        buff[2 * i + 1] = theme;
    }
    offset = 0;
}

//设置主题
void ttyTheme(char fg, char bg) {
    if ((0 <= fg && fg <= 15) && (0 <= bg && bg <= 15))
        theme = ((bg << 4) & 0xF0) | (fg & 0x0F);
}
