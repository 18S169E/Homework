#include "TESTscan.h"

#include<map>
#include<stdio.h>
#include<ctype.h>
extern int TESTscan();

char Scanin[300], Scanout[300];
FILE* fin, * fout;
#define LL_USE

#ifdef LL_USE
#include "LLparse.h"
extern int LLPARSE();
#define PARSE LLPARSE
#else
#include "TESTparse.h"
extern int TESTparse();
#define PARSE TESTparse
#endif // LL_USE

map<int, int> word2line;
int main() {
	strcpy(Scanin, "input.txt");
	strcpy(Scanout, "output.txt");
	
	//if (1 == 1) {
	//	int es = 0;
	//	//es = TESTscan();
	//	if (es > 0) printf("词法分析有错，编译终止！\n");
	//	else printf("词法分析成功！\n");

	//	
	//	//return 0;
	//	if (es == 0)
	//	{
	//		es = PARSE();
	//		if (es > 0) printf("语法分析有错，编译终止！\n");
	//		else printf("语法分析成功！\n");
	//	}
	//	return 0;
	//}
	//else {
	//	int es = 0;
	//	printf("词法分析成功！\n");
	//	if (es == 0)
	//	{
	//		es = PARSE();
	//		if (es > 0) printf("语法分析有错，编译终止！\n");
	//		else printf("语法分析成功！\n");
	//	}
	//	return 0;
	//}

	LLparse parse;
	vector<string> v({ "id","+","id","*","(","id","+","id",")" });
	//vector<string> v({ "id","+","id","*","id","$"});
	parse.make_tree = true;
	parse.input_file = "production.txt";
	parse.parse(v);
	cout << "语法树为:" << endl;
	parse.grammer_tree->Print(0);
	cout << parse.line << endl;


}