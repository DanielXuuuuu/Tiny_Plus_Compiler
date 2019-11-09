// 词法分析器：得到token序列
// TODO:词法错误检查

#include "util.h"
#include "lexer.h"

#define BUFLEN 256

static char lineBuf[BUFLEN];
static int lineLength = 0;
static int EOF_flag = FALSE;

char tokenString[MAXTOKENLEN + 1];

// DFA的状态
typedef enum {
	STATUS_START,
	STATUS_ASSIGN,
	STATUS_EQUAL,
	STATUS_UNEQUAL,
	STATUS_COMMENT,
	STATUS_NUMBER,
	STATUS_REAL,
	STATUS_STR,
	STATUS_ID,
	STATUS_DONE 
}lexStatus;

static struct
{
	char* str;
	TokenType token;
} reservedWords[RESERVED_NUM]
= { {"IF", IF}, {"ELSE", ELSE},
	{"WHILE", WHILE}, {"DO",DO},
	{"FOR", FOR}, {"UPTO", UPTO}, {"DOWNTO", DOWNTO},
	{"READ", READ}, {"WRITE", WRITE},
	{"BEGIN", BEGIN}, {"END", END},
	{"MAIN", MAIN}, {"RETURN", RETURN},
	{"INT", INT}, {"REAL", REAL}};

static char getNextChar(void) {
	if (columnNo >= lineLength) {
		// 一次读入一行
		if (fgets(lineBuf, BUFLEN - 1, source)) {
			lineNo++;
			lineLength = strlen(lineBuf);
			columnNo = 0;
			return lineBuf[columnNo++];
		}
		else {
			EOF_flag = TRUE;
			return EOF;
		}
	}
	else
		return lineBuf[columnNo++];
}

static void undoGetNextChar(void) {
	if (!EOF_flag) columnNo--;
}

// 查找是否是保留字
static TokenType reservedLookup(char* str) {
	int i;
	for (i = 0; i < RESERVED_NUM; i++) {
		if (!strcmp(str, reservedWords[i].str))
			return reservedWords[i].token;
	}
	return ID;
}

static void lexerError()
{
	fprintf(stderr, "\n[ERROR] Something wrong around line %d, column %d. Please fix it first!\n", lineNo, columnNo);
	fprintf(output, "\n[INFO] Press enter to exit.");
	getchar();
	exit(1);
}

// 词法分析主函数
TokenType getToken(void) {
	// 记录字符位置
	int tokenStringIndex = 0;
	// 记录当前分析得到的token
	TokenType currentToken;
	// 记录当前状态，初始为START
	lexStatus currentStatus = STATUS_START;
	// 记录当前字符是否需要保存下来
	int save;

	while (currentStatus != STATUS_DONE) {
		// 获得下一个字符
		char c = getNextChar();
		save = TRUE;
		// 状态转移
		switch (currentStatus) 
		{
		case STATUS_START:
			if (isdigit(c))
				currentStatus = STATUS_NUMBER;
			else if (isalpha(c)) 
				currentStatus = STATUS_ID;
			else if (c == '"') {
				save = FALSE;
				currentStatus = STATUS_STR;
			}
			else if (c == ':')
				currentStatus = STATUS_ASSIGN;
			else if (c == '=')
				currentStatus = STATUS_EQUAL;
			else if (c == '!')
				currentStatus = STATUS_UNEQUAL;
			else if (c == ' ' || c == '\t' || c == '\n')
				//忽略空格、制表符、换行符
				save = FALSE;
			else if (c == '/') {
				if (getNextChar() == '*') {
					save = FALSE;
					currentStatus = STATUS_COMMENT;
				}
				else
					undoGetNextChar();
			}
			else
			{
				currentStatus = STATUS_DONE;
				switch (c)
				{
				case EOF:
					save = FALSE;
					currentToken = ENDFILE;
					break;
				case '+':
					currentToken = ADD;
					break;
				case '-':
					currentToken = SUB;
					break;
				case '*':
					currentToken = MUL;
					break;
				case '/':
					currentToken = DIV;
					break;
				case '%':
					currentToken = MOD;
					break;
				case '<':
					currentToken = LT;
					break;
				case '>':
					currentToken = GT;
					break;
				case ',':
					currentToken = COMMA;
					break;
				case ';':
					currentToken = SEMI;
					break;
				case '(':
					currentToken = LEFTPAREN;
					break;
				case ')':
					currentToken = RIGHTPAREN;
					break;
				default:
					currentToken = ERROR;
					break;
				}
			}
			break;
		case STATUS_ASSIGN:
			currentStatus = STATUS_DONE;
			if (c == '=')
				currentToken = ASSIGN;
			else {
				undoGetNextChar();
				save = FALSE;
				currentToken = ERROR;
			}
			break;
		case STATUS_EQUAL:
			currentStatus = STATUS_DONE;
			if (c == '=')
				currentToken = EQUAL;
			else {
				undoGetNextChar();
				save = FALSE;
				currentToken = ERROR;
			}
			break;
		case STATUS_UNEQUAL:
			currentStatus = STATUS_DONE;
			if (c == '=')
				currentToken = UNEQUAL;
			else {
				undoGetNextChar();
				save = FALSE;
				currentToken = ERROR;
			}
			break;
		case STATUS_COMMENT:
			save = FALSE;
			if (c == '/') {
				if (c == EOF)
				{
					currentStatus = STATUS_DONE;
					currentToken = ENDFILE;
				}
				// 注释结束,回到初始状态
				currentStatus = STATUS_START;
			}
			break;
		case STATUS_ID:
			if (!isalpha(c) && !isdigit(c)){
				// 取到字符发现不是字母，也不是数字，说明ID读取完毕，回退一步
				undoGetNextChar();
				save = FALSE;
				currentStatus = STATUS_DONE;
				currentToken = ID;
			}
			break;
		case STATUS_NUMBER:
			if (!isdigit(c)) {
				// 取到字符发现不是数字，判断是不是小数点,是小数点，进入STATUS_REAL状态，否则结束
				if (c == '.') {
					currentStatus = STATUS_REAL;
				}
				else if (isalpha(c)) {
					undoGetNextChar();
					save = FALSE;
					currentStatus = STATUS_DONE;
					currentToken = ERROR;
				}
				else {
					undoGetNextChar();
					save = FALSE;
					currentStatus = STATUS_DONE;
					currentToken = NUM;
				}
			}
			break;
		case STATUS_REAL:
			if (!isdigit(c)) {
				// 取到字符发现不是数字，结束
				undoGetNextChar();
				save = FALSE;
				currentStatus = STATUS_DONE;
				currentToken = NUM;
			}
			break;
		case STATUS_DONE:break;
		case STATUS_STR:
			save = TRUE;
			if (c == '"') {
				save = FALSE;
				currentStatus = STATUS_DONE;
				currentToken = QSTR;
			}
			break;
		default:
			// 未定义状态，意味着出现了bug
			currentStatus = STATUS_DONE;
			currentToken = ERROR;
			break;
		}
		if(save && tokenStringIndex < MAXTOKENLEN)
			tokenString[tokenStringIndex++] = c;

		if (currentStatus == STATUS_DONE) {
			if (currentToken == ERROR) {
				lexerError();
			}
			tokenString[tokenStringIndex] = '\0';
			if (currentToken == ID) {
				currentToken = reservedLookup(tokenString);
			}
		}
	}
	if(traceLexer && currentToken != ENDFILE)
		printToken(currentToken, tokenString);
	return currentToken;
}