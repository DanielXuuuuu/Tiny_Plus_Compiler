#include "global.h"
#include "util.h"
#include "lexer.h"
#include "parser.h"

int lineNo = 0;
int columnNo = 0;
FILE * source;
FILE * output;

int traceLexer = FALSE;	
int choice;
char sourcefilePath[120];

#define TEST TRUE

int main() {
	output = stdout;
	menu();
	scanf("%d", &choice);
	while (choice < 0 || choice > 2) {
		printf("[ERROR] Wrong choice! Try again: ");
		scanf("%d", &choice);
	}
	getchar();
	if (choice == 0) {
		printf("Bye!\n");
	}
	else {
#if TEST
		strcpy(sourcefilePath, "C:\\mycode\\test.tiny"); //ÎÄ¼þÂ·¾¶
#else
		printf("[INFO] Please Enter source file path: ");
		scanf("%s", &sourcefilePath);
#endif
		source = fopen(sourcefilePath, "r");
		if (source == NULL) {
			fprintf(stderr, "[ERROR] File not found. Please check and try later!\n");
		}
		if (choice == 1) {
			fprintf(output, "[INFO] Lexical analysis start.\n\n");
			traceLexer = TRUE;
			while (getToken() != ENDFILE);
			fprintf(output, "\n[INFO] Lexical analysis finished.\n");
		}
		else {
			fprintf(output, "[INFO] Syntax analysis start.\n\n");
			TreeNode * syntaxTree;
			syntaxTree = parse();
			fprintf(output, "\n[INFO] Syntax analysis finished! Syntax tree is as below:\n\n");
			printTree(syntaxTree);
			fprintf(output, "\n");
		}
		fclose(source);
	}
	system("pause");
	return 0;
}