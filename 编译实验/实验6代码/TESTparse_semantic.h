#define _CRT_SECURE_NO_WARNINGS

using namespace std;
#include <stack>
#include <map>
#include <assert.h>
#include <stdio.h>
#include <ctype.h>
#include <conio.h>
#include <string.h>



int TESTparse();
int program();
int compound_stat();
int statement();
int expression_stat();
int expression();
int bool_expr();
int additive_expr();
int term();
int factor();
int if_stat();
int while_stat();
int for_stat();
int write_stat();
int read_stat();
int declaration_stat();
int declaration_list();
int statement_list();
int compound_list();
int R();
int R1();
int name_def(char* buff);
int LOOKUP(char* buff);
int BRF(int label);
int BR(int label);
int SETLabel(int label);
char token[20], token1[40]; extern char Scanout[300];
FILE* fp;
FILE* asm_file;

//符号表
#define max_var_num  (320)
struct {
	char name[16];
	int address;
}vartable[max_var_num];
int datap = 0;
int offset = 0;
int label_num = 1;


void gettoken() {
	fscanf(fp, "%s %s\n", token, token1);
}

void printtoken() {
	printf("%s\t%s\n", token, token1);
}

void gandp() {
	gettoken();
	printtoken();
}

//es>0为失败,不同的值对应不同的错误,且每个非叶子函数返回子函数的es

//每次读取都是一行
//1
int program() {
	//cout << "--------progam---------\n";
	gandp();
	if (strcmp(token, "{") == 0) {
		int es;
		gandp();
		es = declaration_list();
		if (es > 0) return es;
		es = statement_list();
		if (es > 0) return es;
		if (strcmp(token, "}") != 0) return 1;
		else return 0;
	}
	else return 1;
}

//消除左递归 dl->R R->dsR|e
int declaration_list() {
	//cout << "--------declaration_list---------\n";
	int es;
	es = R();
	return es;
}

int R() {
	//cout << "-------- R---------\n";
	int es;
	es = declaration_stat();
	if (es > 0) {
		return 0;
	}
	else {
		return R();
	}
	//空的话，什么都不做而且也不会错
}

int declaration_stat() {
	//cout << "-------- declaration_stat---------\n";
	int es;
	if (strcmp(token, "int") == 0) {
		gandp();

		//语义部分
		{
			es = name_def(token1);
			if (es > 0) return es;
		}

		gandp();
		if (strcmp(token, ";") == 0) {
			gandp();
			return 0;
		}
		else return 2;
	}
	else return 2;
}

//消除左递归 dl->R1 R1->sR|e
int statement_list() {
	//cout << "-------- statement_list---------\n";
	int es;
	es = R1();
	return es;
}

int R1() {
	//cout << "-------- R1---------\n";
	int es;
	es = statement();
	if (es > 0) {
		return 0;
	}
	else {
		return R1();
	}
}

int statement() {
	//cout << "-------- statement---------\n";
	int es;
	int hasExecuted = 0;  // 用于跟踪是否已经执行了一个语句

	// 定义函数指针数组，包含需要处理的函数
	int (*stmt_functions[])() = { if_stat, while_stat, for_stat, read_stat, write_stat, compound_stat, expression_stat };
	int num_functions = sizeof(stmt_functions) / sizeof(stmt_functions[0]);

	// 遍历函数列表并执行相同的操作
	for (int i = 0; i < num_functions; i++) {
		es = stmt_functions[i]();
		if (es == 0) {
			//gandp();
			hasExecuted = 1;
			break;  // 如果成功执行一个语句，则退出循环
		}
	}

	return hasExecuted ? 0 : es;
}

//6
int if_stat() {
	//cout << "-------- if_stat---------\n";
	int es;
	if (strcmp(token, "if") == 0) {
		gandp();
		if (strcmp(token, "(") == 0) {
			gandp();
			es = expression();
			if (es > 0) return es;
			if (strcmp(token, ")") == 0) {
				int label1 = label_num++;
				BRF(label1);
				gandp();
				es = statement();
				int label2 = label_num++;
				BR(label2);
				SETLabel(label1);
				if (es > 0) return es;
				if (strcmp(token, "else") == 0) {
					gandp();
					es = statement();
					SETLabel(label2);
					if (es > 0) return es;
					return 0;
				}
				else {
					//可以为空
					return 0;
				}
			}
			else return 3;
		}
		else return 3;

	}
	else return 3;
}
//<表达式> à <标识符>=<布尔表达式><布尔表达式>
//<expr> := ID = <bool _expr> | <bool_expr>

//7
int while_stat() {
	//cout << "-------- while_stat---------\n";
	int es;
	if (strcmp(token, "while") == 0) {
		int label1 = label_num++;
		int label2 = label_num++;
		SETLabel(label1);
		gandp();
		if (strcmp(token, "(") == 0) {
			gandp();
			es = expression();
			if (es > 0) return es;
			if (strcmp(token, ")") == 0) {
				BRF(label2);
				gandp();
				es = statement();
				BR(label1);
				SETLabel(label2);
				if (es > 0) return es;
				return 0;
			}
			else return 3;
		}
		else return 3;

	}
	else return 3;
}

//8
int for_stat() {
	//cout << "-------- for_stat---------\n";
	int es;
	if (strcmp(token, "for") == 0) {
		int label1 = label_num++;
		int label2 = label_num++;
		int label3 = label_num++;
		int label4 = label_num++;
		gandp();
		if (strcmp(token, "(") == 0) {
			gandp();
			es = expression();
			fprintf(asm_file, "POP\n");
			if (es > 0) return es;
			if (strcmp(token, ";") == 0) {
				SETLabel(label1);
				gandp();
				es = expression();
				if (es > 0) return es;
				BRF(label2);
				BR(label3);
				if (strcmp(token, ";") == 0) {
					SETLabel(label4);
					gandp();
					es = expression();
					if (es > 0) return es;
					fprintf(asm_file, "POP\n");
					BR(label1);
					if (strcmp(token, ")") == 0) {
						SETLabel(label3);
						gandp();
						es = statement();
						if (es > 0) return es;
						BR(label4);
						SETLabel(label2);
						return 0;
					}
					else return 4;
				}
				else return 4;
			}
			else return 4;
		}
		else return 4;
	}
	else return 4;
}

//9
int write_stat() {
	//cout << "-------- write_stat---------\n";
	int es;
	if (strcmp(token, "write") == 0) {
		gandp();
		es = expression();
		if (es > 0) return es;
		fprintf(asm_file, "OUT\n");
		if (strcmp(token, ";") == 0) {
			gandp();
			return 0;
		}
		else return 5;
	}
	else return 5;
}

//10
int read_stat() {
	//cout << "-------- read_stat---------\n";
	int es;
	if (strcmp(token, "read") == 0) {
		gandp();
		if (strcmp(token, "ID") == 0) {
			int address = LOOKUP(token1);
			fprintf(asm_file, "IN\n");
			fprintf(asm_file, "STO %d\n", address);
			fprintf(asm_file, "POP\n");
			gandp();
			if (strcmp(token, ";") == 0) {
				gandp();
				return 0;
			}
			else return 6;
		}
		else return 6;
	}
	else return 6;
}

//11
int compound_stat() {
	//cout << "-------- compound_stat---------\n";
	int es;
	if (strcmp(token, "{") == 0) {
		gandp();
		es = statement_list();
		if (es > 0) return es;
		if (strcmp(token, "}") == 0) {
			gandp();
			return 0;
		}
		else return 7;
	}
	else return 7;
}

//12
int expression_stat() {
	//cout << "-------- expression_stat---------\n";
	int es;
	if (strcmp(token, ";") == 0) {
		gandp();
		return 0;
	}
	else {
		es = expression();
		if (es > 0) return es;
		fprintf(asm_file, "POP\n");
		if (es > 0) return es;
		if (strcmp(token, ";") == 0) {
			gandp();
			return 0;
		}
		else return 8;
	}
}

//13
int expression()
{
	//cout << "-------- expression---------\n";
	int es = 0, fileadd;
	char token2[20], token3[40];
	if (strcmp(token, "ID") == 0)
	{
		fileadd = ftell(fp); //记住当前文件位置
		fscanf(fp, "%s %s\n", token2, token3);
		printf("%s\t%s \n", token2, token3);

		int address = LOOKUP(token1);
		if (address == -1) return 103;
		if (strcmp(token2, "=") == 0) //赋值表达式
		{
			fscanf(fp, "%s %s\n", token, token1);
			printf("%s\t%s\n", token, token1);

			

			es = bool_expr();
			fprintf(asm_file, "STO %d\n", address);
			if (es > 0)return(es);
		}
		else
		{
			fseek(fp, fileadd, 0); //若非=, 则文件指针回到=前的标识符
			printf("%s\t%s\n", token, token1);
			es = bool_expr();
			if (es > 0) return(es);
		}
	}
	else es = bool_expr();
	return(es);
}

//14
int bool_expr()
{
	//cout << "-------- bool_expr---------\n";
	int es = 0;
	es = additive_expr();
	if (es > 0) return(es);
	if (strcmp(token, ">") == 0 || strcmp(token, "<") == 0 || strcmp(token, ">=") == 0 || strcmp(token, "<=") == 0 || strcmp(token, "==") == 0 || strcmp(token, "!=") == 0)
	{
		int op = strcmp(token, ">") == 0 ? 1 : strcmp(token, "<") == 0 ? 2 : strcmp(token, ">=") == 0 ? 3 : strcmp(token, "<=") == 0 ? 4 : strcmp(token, "==") == 0 ? 5 : strcmp(token, "!=") == 0 ? 6 : 0;
		gandp();
		es = additive_expr();

		switch (op) {
		case 1: fprintf(asm_file, "GT\n"); break;
		case 2: fprintf(asm_file, "LES\n"); break;
		case 3: fprintf(asm_file, "GE\n"); break;
		case 4: fprintf(asm_file, "LE\n"); break;
		case 5: fprintf(asm_file, "EQ\n"); break;
		case 6: fprintf(asm_file, "NOTEQ\n"); break;
		};

		if (es > 0) return(es);
	}
	return 0;
}

//15
int additive_expr()
{
	//cout << "-------- additive_expr---------\n";
	int es = 0;
	es = term();
	if (es > 0) return(es);
	if (strcmp(token, "+") == 0 || strcmp(token, "-") == 0)
	{
		int op = strcmp(token, "+") == 0 ? 1 : strcmp(token, "-") == 0 ? 2 : 0;
		gandp();
		es = term();
		switch (op) {
		case 1: fprintf(asm_file, "ADD\n"); break;
		case 2: fprintf(asm_file, "SUB\n"); break;
		};

		
		if (es > 0) return(es);
	}
	return 0;
}

//16
int term()
{
	//cout << "-------- term---------\n";

	int es = 0;
	es = factor();
	if (es > 0) return(es);
	if (strcmp(token, "*") == 0 || strcmp(token, "/") == 0)
	{
		int op = strcmp(token, "*") == 0 ? 1 : strcmp(token, "/") == 0 ? 2 : 0;
		gandp();
		es = factor();
		switch (op) {
		case 1: fprintf(asm_file, "MULT\n"); break;
		case 2: fprintf(asm_file, "DIV\n"); break;
		};
		if (es > 0) return(es);
	}
	return 0;
}

//17
int factor()
{
	//cout << "-------- factor---------\n";
	int es = 0;
	if (strcmp(token, "(") == 0) {
		gandp();
		es = expression();
		if (es > 0) return(es);
		if (strcmp(token, ")") == 0) {
			gandp();
			return 0;
		}
		else {
			return 13;
		}
	}
	else if (strcmp(token, "ID") == 0)
	{
		int address = LOOKUP(token1);
		fprintf(asm_file, "LOAD %d\n", address);
		gandp();
		return 0;
	}
	else if (strcmp(token, "NUM") == 0)
	{
		int num = atoi(token1);
		fprintf(asm_file, "LOADI %d\n", num);
		gandp();
		return 0;
	}
	else return 13;
}


//定义相关函数
int name_def(char* buff) {
	if (datap >= max_var_num) {
		return 101;
	}
	for (int i = 0; i < max_var_num; i++) {
		if (strcmp(vartable[i].name, buff) == 0) {
			return 102;
		}
	}
	strcpy(vartable[datap].name, buff);
	vartable[datap++].address = offset;
	offset += 2;

	return 0;
}

int LOOKUP(char* buff) {
	for (int i = 0; i < max_var_num; i++) {
		if (strcmp(vartable[i].name, buff) == 0) {
			return vartable[i].address;
		}
	}
	return -1;
}


int BRF(int label) {
	//TODO 跳转的实现
	fprintf(asm_file, "BRF LABEL%d\n", label);
	return 0;
}

int BR(int label) {
	//TODO 跳转的实现
	fprintf(asm_file, "BR LABEL%d\n", label);
	return 0;
}

int SETLabel(int label) {
	//TODO 跳转的实现
	fprintf(asm_file, "LABEL%d:\n", label);
	return 0;
}

//语法分析程序
int TESTparse() {
	int es = 0;
	strcpy(Scanout, "output.txt");
	if ((fp = fopen(Scanout, "r")) == NULL)
	{
		printf("\n Can not open % S\n", Scanout);
		es = 10;
	}
	asm_file = fopen("asm.txt", "w");
	if (es == 0) es = program();
	printf(" == 语法分析结果 == \n"); switch (es)
	{
	case 0: printf("语法分析成功\n"); break;
	case 11: printf("打开文件 % s失败", Scanout); break;
	default:printf("错误值为:%d\n", es);
		//case 1: printf("1\n"); break;
		//case 2: printf("2\n"); break;
		//case 3: printf("3\n"); break;
		//case 4: printf("4\n"); break;
		//case 5: printf("\n"); break;
		//case 6: printf("\n"); break;
		//case 7: printf("\n"); break;
	}
	fclose(asm_file);
	return(es);
}