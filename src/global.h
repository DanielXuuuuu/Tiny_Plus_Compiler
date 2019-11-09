#define _CRT_SECURE_NO_WARNINGS

#ifndef GLOBAL_H
#define GLOBAL_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#define FALSE 0
#define TRUE 1
#define RESERVED_NUM 15

// 文件指针
extern FILE* source;
extern FILE* output;

// 用于定位错误点
extern int lineNo;
extern int columnNo;

// token类型
typedef enum {
	// 保留字
	IF, ELSE, WRITE, READ, RETURN, BEGIN, END, MAIN, INT, REAL, WHILE, DO, FOR, UPTO, DOWNTO,

	// 分隔符
	SEMI, COMMA, LEFTPAREN, RIGHTPAREN,

	// 运算符
	ADD, SUB, MUL, DIV, MOD, ASSIGN, EQUAL, UNEQUAL, GT, LT,
	
	// ID NUM
	ID, NUM, QSTR,

	// other
	ERROR, ENDFILE
}TokenType;

#define MAXCHILDREN 10

typedef enum { MethodK, TypeK, ParamK, StmtK, ExpK } NodeKind;
typedef enum { MainK, NormalK } MethodKind;
typedef enum { FormalK, ActualK, NoneK } ParamKind;
typedef enum { ReturnTypeK, IntTypeK, RealTypeK } TypeKind;
typedef enum { IfK, WhileK, DoWhileK, ForK, ReturnK, AssignK, ReadK, WriteK, IntDeclareK, RealDeclareK } StmtKind;
typedef enum { OpK, ConstK, IdK, MethodCallK} ExpKind;

// 语法分析树结构体 - 多叉树
typedef struct
{
	struct TreeNode* child[MAXCHILDREN];         // 当前函数子树
	struct TreeNode* sibling;                    // 指向下一个函数
	NodeKind nodekind;                           // 节点类型
	union { MethodKind method; TypeKind type; StmtKind stmt; ExpKind exp; } kind;  // 子类型
	union {
		TokenType token;
		float val;
		char * name;
	} attr;
}TreeNode;

extern int traceLexer;

#endif // !GLOBAL_H