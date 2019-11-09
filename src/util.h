#ifndef UTIL_H
#define UTIL_H

#include "global.h"

void printToken(TokenType token, const char* str);

TreeNode* NewMethodNode(MethodKind);
TreeNode* NewTypeNode(TypeKind);
TreeNode* NewParamNode(TypeKind);
TreeNode* newStmtNode(StmtKind);
TreeNode* newExpNode(ExpKind);

void printTree(TreeNode*);

char* copyString(char*);
void printSpaces(void);

void menu(void);
#endif // !UTIL_H
