#ifndef LEXER_H
#define LEXER_H

#include "global.h"

#define MAXTOKENLEN 255
extern char tokenString[MAXTOKENLEN + 1]; //���ڱ��������ַ�

TokenType getToken(void);
#endif // !LEXER_H
