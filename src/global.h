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

// �ļ�ָ��
extern FILE* source;
extern FILE* output;

// ���ڶ�λ�����
extern int lineNo;
extern int columnNo;

// token����
typedef enum {
	// ������
	IF, ELSE, WRITE, READ, RETURN, BEGIN, END, MAIN, INT, REAL, WHILE, DO, FOR, UPTO, DOWNTO,

	// �ָ���
	SEMI, COMMA, LEFTPAREN, RIGHTPAREN,

	// �����
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

// �﷨�������ṹ�� - �����
typedef struct
{
	struct TreeNode* child[MAXCHILDREN];         // ��ǰ��������
	struct TreeNode* sibling;                    // ָ����һ������
	NodeKind nodekind;                           // �ڵ�����
	union { MethodKind method; TypeKind type; StmtKind stmt; ExpKind exp; } kind;  // ������
	union {
		TokenType token;
		float val;
		char * name;
	} attr;
}TreeNode;

extern int traceLexer;

#endif // !GLOBAL_H