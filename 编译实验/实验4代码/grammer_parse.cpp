#include "TESTscan.h"
#include "TESTparse.h"
#include<stdio.h>
#include<ctype.h>
extern int TESTscan();
extern int TESTparse();
char Scanin[300], Scanout[300];
FILE* fin, * fout;
int main() {
	if (1 == 1) {
		int es = 0;
		es = TESTscan();
		if (es > 0) printf("词法分析有错，编译终止！\n");
		else printf("词法分析成功！\n");
		if (es == 0)
		{
			es = TESTparse();
			if (es > 0) printf("语法分析有错，编译终止！\n");
			else printf("语法分析成功！\n");
		}
		return 0;
	}
	else {

		int es = 0;
		printf("词法分析成功！\n");
		if (es == 0)
		{
			es = TESTparse();
			if (es > 0) printf("语法分析有错，编译终止！\n");
			else printf("语法分析成功！\n");
		}
		return 0;
	}




}