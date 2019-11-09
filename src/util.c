#include "global.h"
#include "util.h"

static int deepthOfTree = 0;

#define INCRESEDEEP deepthOfTree++;
#define DECRESEDEEP deepthOfTree--;

void printToken(TokenType token, const char* str) {
	switch (token) {
	case IF:
	case ELSE: 
	case WHILE:
	case DO:
	case FOR:
	case UPTO:
	case DOWNTO:
	case WRITE: 
	case READ: 
	case RETURN: 
	case BEGIN: 
	case END:
	case MAIN: 
	case INT:
	case REAL:
		fprintf(output, "(KEYWORD, %s)\n", str); break;
	
	case SEMI: fprintf(output, "(SEP, ;)\n"); break;
	case COMMA: fprintf(output, "(SEP, ,)\n"); break;
	case LEFTPAREN: fprintf(output, "(SEP, ()\n"); break;
	case RIGHTPAREN: fprintf(output, "(SEP, ))\n"); break;

	case ADD: fprintf(output, "(OP, +)\n"); break;
	case SUB: fprintf(output, "(OP, -)\n"); break;
	case MUL: fprintf(output, "(OP, *)\n"); break;
	case DIV: fprintf(output, "(OP, /)\n"); break;
	case MOD: fprintf(output, "(OP, %%)\n"); break;
	case ASSIGN: fprintf(output, "(OP, :=)\n"); break;
	case EQUAL: fprintf(output, "(OP, ==)\n"); break;
	case UNEQUAL: fprintf(output, "(OP, !=)\n"); break;
	case GT: fprintf(output, "(OP, >)\n"); break;
	case LT: fprintf(output, "(OP, <)\n"); break;

	case ID: fprintf(output, "(ID, %s)\n", str); break;
	case NUM: fprintf(output,"(NUM, %s)\n", str); break;
	case QSTR: fprintf(output, "(QString, %s)\n", str); break;

	//case ERROR:
	case ENDFILE: fprintf(output, "EOF\n"); break;
	default:
		fprintf(output, "Unknown token: %d\n", token);
	}
}

TreeNode* NewMethodNode(MethodKind kind) {
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
	if (t == NULL) {
		fprintf(output, "Out of memory error at line %d\n", lineNo);
	}
	else {
		int i;
		for (i = 0; i < MAXCHILDREN; i++) {
			t->child[i] = NULL;
			t->sibling = NULL;
			t->nodekind = MethodK;
			t->kind.stmt = kind;
		}
	}
	return t;
}

TreeNode* NewTypeNode(TypeKind kind) {
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
	if (t == NULL) {
		fprintf(output, "Out of memory error at line %d\n", lineNo);
	}
	else {
		int i;
		for (i = 0; i < MAXCHILDREN; i++) {
			t->child[i] = NULL;
			t->sibling = NULL;
			t->nodekind = TypeK;
			t->kind.stmt = kind;
		}
	}
	return t;
}

TreeNode* NewParamNode(ParamKind kind) {
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
	if (t == NULL) {
		fprintf(output, "Out of memory error at line %d\n", lineNo);
	}
	else {
		int i;
		for (i = 0; i < MAXCHILDREN; i++) {
			t->child[i] = NULL;
			t->sibling = NULL;
			t->nodekind = ParamK;
			t->kind.stmt = kind;
		}
	}
	return t;
}

TreeNode* newStmtNode(StmtKind kind) {
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
	if (t == NULL) {
		fprintf(output, "Out of memory error at line %d\n", lineNo);
	}
	else {
		int i;
		for (i = 0; i < MAXCHILDREN; i++) {
			t->child[i] = NULL;
			t->sibling = NULL;
			t->nodekind = StmtK;
			t->kind.stmt = kind;
		}
	}
	return t;
}

TreeNode* newExpNode(ExpKind kind) {
	TreeNode* t = (TreeNode*)malloc(sizeof(TreeNode));
	if (t == NULL) {
		fprintf(output, "Out of memory error at line %d\n", lineNo);
	}
	else {
		int i;
		for (i = 0; i < MAXCHILDREN; i++) {
			t->child[i] = NULL;
			t->sibling = NULL;
			t->nodekind = ExpK;
			t->kind.stmt = kind;
		}
	}
	return t;
}

void printTree(TreeNode* root) {
	int i;
	INCRESEDEEP;
	while (root != NULL) {
		printSpaces();
		if (root->nodekind == MethodK) {
			switch (root->kind.method)
			{
			case MainK:
				fprintf(output, "Main Method: %s\n", root->attr.name);
				break;
			case NormalK:
				fprintf(output, "Method: %s\n", root->attr.name);
				break;
			default:
				break;
			}
		}
		else if (root->nodekind == TypeK) {
			switch (root->kind.type) 
			{
			case ReturnTypeK:
				fprintf(output, "Return Type: %s\n", root->attr.name); break;
			case IntTypeK: 
				fprintf(output, "INT: %s\n",root->attr.name); break;
			case RealTypeK:
				fprintf(output, "REAL: %s\n", root->attr.name); break;
			default:
				fprintf(output, "Unknown StmtNode kind\n");
				break;
			}
		}
		else if (root->nodekind == ParamK)
		{
			switch (root->kind.stmt)
			{
			case FormalK:
				fprintf(output, "FormalParam:\n"); break;
			case ActualK:
				fprintf(output, "ActualParam:\n"); break;
			default:
				fprintf(output, "Unknown StmtNode kind\n");
				break;
			}
		}
		else if (root->nodekind == StmtK) 
		{
			switch (root->kind.stmt)
			{
			case IfK:
				fprintf(output, "If\n"); break;
			case WhileK:
				fprintf(output, "While\n"); break;
			case DoWhileK:
				fprintf(output, "DoWhile\n"); break;
			case ForK:
				fprintf(output, "For\n"); break;
			case ReturnK:
				fprintf(output, "Return\n"); break;
			case AssignK:
				fprintf(output, "Assign to: %s\n", root->attr.name); break;
			case ReadK:
				fprintf(output, "Read form \"%s\"\n", root->attr.name); break;
			case WriteK:
				fprintf(output, "Write to \"%s\"\n", root->attr.name); break;
			case IntDeclareK:
				fprintf(output, "LocalVarDecl: INT %s\n", root->attr.name); break;
			case RealDeclareK:
				fprintf(output, "LocalVarDecl: REAL %s\n", root->attr.name); break;
			default:
				fprintf(output, "Unknown StmtNode kind\n");
				break;
			}
		}
		else if (root->nodekind == ExpK) {
			switch (root->kind.exp)
			{
			case OpK:
				fprintf(output, "Op: ");
				printToken(root->attr.token, '\0');
				break;
			case ConstK:
				fprintf(output, "Const: %f\n", root->attr.val);
				break;
			case IdK:
				fprintf(output, "Id: %s\n", root->attr.name);
				break;
			case MethodCallK:
				fprintf(output, "function Call: %s\n", root->attr.name);
				break;
			default:
				fprintf(output, "Unknown ExpNode kind\n");
				break;
			}
		}
		else
			fprintf(output, "Unknown node kind\n");
		for (i = 0; i < MAXCHILDREN; i++)
			printTree(root->child[i]);
		
		root = root->sibling;
	}
	DECRESEDEEP;
}

// 字符串拷贝
char* copyString(char* str) {
	if (str == NULL) return NULL;
	int n;
	char* res;
	n = strlen(str) + 1;
	res = (char*)malloc(n);
	if (res == NULL) {
		fprintf(output, "Out of memory error at line %d\n", lineNo);
	}
	else
	{
		strcpy(res, str);
	}
	return res;
}

// 打印空格，通过缩进表示语法树的层次
void printSpaces(void) {
	int i;
	for (i = 0; i < deepthOfTree * 2; i++)
		fprintf(output, " ");
}

void menu(void) {
	fprintf(output, "======================= TINY+  COMPILER =======================\n");
	fprintf(output, "                     1. Lexical analysis\n");
	fprintf(output, "                     2. Syntax analysis\n");
	fprintf(output, "                     0. Quit\n");
	fprintf(output, "===============================================================\n");
	fprintf(output, "[INPUT] Please enter your choice: ");
}