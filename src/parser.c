// 语法分析器：构建语法分析树

#include "lexer.h"
#include "parser.h"
#include "util.h"

static TokenType token; //储存当前的token

// 匹配函数
static TreeNode* MethodDecl(void);
static TreeNode* MethodDecl_Sequence(void);
static TreeNode* ReturnType(void);
static TreeNode* IntType(void);
static TreeNode* RealType(void);
static TreeNode* Type_Sequence(void);
static TreeNode* FormalParams(void);
static TreeNode* Block(void);
static TreeNode* Statement(void);
static TreeNode* Statement_Sequence(void);
static TreeNode* IntLocalVarDeclStmt(void);
static TreeNode* RealLocalVarDeclStmt(void);
static TreeNode* AssignStmt(void);
static TreeNode* ReturnStmt(void);
static TreeNode* IfStmt(void);
static TreeNode* WhileStmt(void);
static TreeNode* DoWhileStmt(void);
static TreeNode* ForStmt(void);
static TreeNode* WriteStmt(void);
static TreeNode* ReadStmt(void);
static TreeNode* Expression(void);
static TreeNode* MultiplicativeExpr(void);
static TreeNode* PrimaryExpr(void);
static TreeNode* BoolExpression(void);
static TreeNode* ActualParams(void);

static void syntaxError(char* errMessage)
{
	fprintf(stderr, "\n[ERROR] Syntax error around line %d, column %d: %s. Please fix it first!\n", lineNo, columnNo, errMessage);
	fprintf(output, "\n[INFO] Press enter to exit.");
	getchar();
	exit(1);
}

static void match(TokenType expected) {
	if (token == expected) token = getToken();
	else {
		syntaxError("Unexpected token");
	}
}

TreeNode* parse(void) {
	TreeNode * root;
	token = getToken();
	root = MethodDecl_Sequence();
	if (token != ENDFILE) {
		syntaxError("Code ends before file");
	}
	return root;
}

static TreeNode * MethodDecl(void) {
	TreeNode* t = NULL;
	TreeNode* p = ReturnType();

	if (token == MAIN) {
		t = NewMethodNode(MainK);
		match(MAIN);
	}
	else
		t = NewMethodNode(NormalK);
	t->child[0] = p;
	//函数名
	if (t != NULL && token == ID) {
		t->attr.name = copyString(tokenString);
	}
	match(ID);
	match(LEFTPAREN);
	if (token == RIGHTPAREN) {
		t->child[1] = NULL;
		match(RIGHTPAREN);
		t->child[2] = Block();
	}
	else {
		t->child[1] = FormalParams();
		match(RIGHTPAREN);
		t->child[2] = Block();
	}
	return t;	
}

// 程序主体由函数组成
static TreeNode * MethodDecl_Sequence(void) {
	TreeNode* t = MethodDecl();
	TreeNode* p = t;
	while (token != ENDFILE) {
		TreeNode* q;
		q = MethodDecl();
		if (q != NULL) {
			if (t == NULL) t = p = q;
			else {
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}

static TreeNode * ReturnType(void) {
	TreeNode* t = NewTypeNode(ReturnTypeK);
	if (token != INT && token != REAL) {
		syntaxError("Invalid return type");
	}
	t->attr.name = copyString(tokenString);
	match(token);
	return t;
}

static TreeNode * IntType(void) {
	TreeNode* t = NewTypeNode(IntTypeK);
	match(INT);
	return t;
}

static TreeNode * RealType(void) {
	TreeNode* t = NewTypeNode(RealTypeK);
	match(REAL);
	return t;
}

static TreeNode * Type_Sequence(void) {
	TreeNode* t = NULL;
	switch (token)
	{
	case INT:
		t = NewTypeNode(IntTypeK);
		match(INT);
		break;
	case REAL:
		t = NewTypeNode(RealTypeK);
		match(REAL);
		break;
	default:
		syntaxError("Invalid parameter type");
		break;
	}
	t->attr.name = copyString(tokenString);
	match(ID);
	TreeNode* p = t;
	while (token == COMMA) {
		match(COMMA);
		TreeNode* q;
		q = Type_Sequence();
		if (q != NULL) {
			if (t == NULL) t = p = q;
			else {
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}

static TreeNode * FormalParams(void) {
	TreeNode* t = NewParamNode(FormalK);
	t->child[0] = Type_Sequence();
	return t;
}

// 函数主体由很多语句组成
static TreeNode* Block(void) {
	match(BEGIN);
	TreeNode* t = Statement_Sequence();
	match(END);
	return t;
}

static TreeNode* Statement(void) {
	TreeNode* t = NULL;
	switch (token) {
	case BEGIN: t = Block(); break;
	case INT: t = IntLocalVarDeclStmt(); break;
	case REAL: t = RealLocalVarDeclStmt(); break;
	case ID: t = AssignStmt(); break;
	case RETURN: t = ReturnStmt(); break;
	case IF: t = IfStmt(); break;
	case WHILE: t = WhileStmt(); break;
	case DO: t = DoWhileStmt(); break;
	case FOR: t = ForStmt(); break;
	case WRITE: t = WriteStmt(); break;
	case READ: t = ReadStmt(); break;
	default:
		syntaxError("Unexpected token");
		break;
	}
	return t;
}

static TreeNode* Statement_Sequence(void) {
	TreeNode * t = Statement();
	TreeNode * p = t;
	while (token != ENDFILE && token != END){
		TreeNode * q;
		q = Statement();
		if (q != NULL) {
			if (t == NULL) t = p = q;
			else{
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}

static TreeNode* IntLocalVarDeclStmt(void) {
	TreeNode * t = newStmtNode(IntDeclareK);
	match(INT);
	if (t != NULL) {
		t->attr.name = copyString(tokenString);
		match(ID);
	}
	match(SEMI);
	return t;
}

static TreeNode* RealLocalVarDeclStmt(void) {
	TreeNode * t = newStmtNode(RealDeclareK);
	match(REAL);
	if (t != NULL) {
		t->attr.name = copyString(tokenString);
		match(ID);
	}
	match(SEMI);
	return t;
}

static TreeNode * AssignStmt(void) {
	TreeNode* t = newStmtNode(AssignK);
	if (t != NULL && token == ID) {
		t->attr.name = copyString(tokenString);
	}
	match(ID);
	match(ASSIGN);
	if (t != NULL) t->child[0] = Expression();
	match(SEMI);
	return t;
}

static TreeNode * ReturnStmt(void) {
	TreeNode* t = newStmtNode(ReturnK);
	match(RETURN);
	if (t != NULL) t->child[0] = Expression();
	match(SEMI);
	return t;
}

static TreeNode * IfStmt(void) {
	TreeNode* t = newStmtNode(IfK);
	match(IF);
	match(LEFTPAREN);
	if (t != NULL) t->child[0] = BoolExpression();
	match(RIGHTPAREN);
	if (t != NULL) t->child[1] = Statement();
	if (token == ELSE) {
		match(ELSE);
		if (t != NULL) t->child[2] = Statement();
	}
	return t;
}

static TreeNode* WhileStmt(void) {
	TreeNode* t = newStmtNode(WhileK);
	match(WHILE);
	match(LEFTPAREN);
	if (t != NULL) t->child[0] = BoolExpression();
	match(RIGHTPAREN);
	if (t != NULL) t->child[1] = Statement();
	return t;
}

static TreeNode* DoWhileStmt(void) {
	TreeNode* t = newStmtNode(DoWhileK);
	match(DO);
	if (t != NULL) t->child[0] = Statement();
	match(WHILE);
	match(LEFTPAREN);
	if (t != NULL) t->child[1] = BoolExpression();
	match(RIGHTPAREN);
	return t;
}

static TreeNode* ForStmt(void) {
	TreeNode* t = newStmtNode(ForK);
	match(FOR);
	if (t != NULL) t->child[0] = AssignStmt();
	if (token == UPTO || token == DOWNTO) {
		match(token);
		if (t != NULL) t->child[1] = Expression();
		match(DO);
		if (t != NULL) t->child[2] = Statement();
	}
	else {
		syntaxError("Invalid writePath");
	}
	return t;
}

static TreeNode * WriteStmt(void) {
	TreeNode* t = newStmtNode(WriteK);
	match(WRITE);
	match(LEFTPAREN);
	if (t != NULL) t->child[0] = Expression();
	match(COMMA);
	if (token != QSTR) {
		syntaxError("Invalid writePath");
	}
	t->attr.name = copyString(tokenString);
	match(QSTR);
	match(RIGHTPAREN);
	match(SEMI);
	return t;
}

static TreeNode * ReadStmt(void) {
	TreeNode* t = newStmtNode(ReadK);
	match(READ);
	match(LEFTPAREN);
	if (t != NULL && token == ID)
		t->child[0] = PrimaryExpr();
	match(COMMA);
	if (token != QSTR) {
		syntaxError("Invalid writePath");
	}
	t->attr.name = copyString(tokenString);
	match(QSTR);
	match(RIGHTPAREN);
	match(SEMI);
	return t;
}

// 算数表达式，以运算符为根节点
static TreeNode * Expression(void) {
	TreeNode* t = MultiplicativeExpr();
	while (token == ADD || token == SUB) {
		TreeNode *p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.token = token;
			t = p;
			match(token);
			t->child[1] = MultiplicativeExpr();
		}
	}
	return t;
}

static TreeNode * MultiplicativeExpr(void) {
	TreeNode* t = PrimaryExpr();
	while (token == MUL || token == DIV || token == MOD) {
		TreeNode *p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.token = token;
			t = p;
			match(token);
			t->child[1] = PrimaryExpr();
		}
	}
	return t;
}

static TreeNode * PrimaryExpr(void) {
	char* temp;
	TreeNode * t = NULL;
	switch (token) {
	case NUM:
		t = newExpNode(ConstK);
		if ((t != NULL) && (token == NUM))
			t->attr.val = atof(tokenString);
		match(NUM);
		break;
	case ID:
		temp = copyString(tokenString);
		match(ID);
		if (token == LEFTPAREN) {
			t = newExpNode(MethodCallK);
			t->attr.name = copyString(temp);
			match(LEFTPAREN);
			t->child[0] = ActualParams();
			match(RIGHTPAREN);
		}
		else {
			t = newExpNode(IdK);
			t->attr.name = copyString(temp);
		}
		break;
	case LEFTPAREN:
		match(LEFTPAREN);
		t = Expression();
		match(RIGHTPAREN);
		break;
	default:
		syntaxError("Unexpected token");
		break;
	}
	return t;
}

static TreeNode * BoolExpression(void) {
	TreeNode* t = Expression();
	if (token == EQUAL || token == UNEQUAL || token == GT || token == LT) {
		TreeNode * p = newExpNode(OpK);
		if (p != NULL) {
			p->child[0] = t;
			p->attr.token = token;
			t = p;
			match(token);
			t->child[1] = Expression();
		}
	}
	else {
		syntaxError("Unexpected token");
	}
	return t;
}

// 实参列表
static TreeNode * ActualParams(void) {
	TreeNode* t = NewParamNode(ActualK);
	TreeNode* e = Expression();
	t->child[0] = e;
	TreeNode* p = e;
	while (token == COMMA) {
		match(COMMA);
		TreeNode* q;
		q = Expression();
		if (q != NULL) {
			if (e == NULL) e = p = q;
			else {
				p->sibling = q;
				p = q;
			}
		}
	}
	return t;
}