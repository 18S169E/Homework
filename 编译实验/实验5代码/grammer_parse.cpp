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
	//	if (es > 0) printf("�ʷ������д�������ֹ��\n");
	//	else printf("�ʷ������ɹ���\n");

	//	
	//	//return 0;
	//	if (es == 0)
	//	{
	//		es = PARSE();
	//		if (es > 0) printf("�﷨�����д�������ֹ��\n");
	//		else printf("�﷨�����ɹ���\n");
	//	}
	//	return 0;
	//}
	//else {
	//	int es = 0;
	//	printf("�ʷ������ɹ���\n");
	//	if (es == 0)
	//	{
	//		es = PARSE();
	//		if (es > 0) printf("�﷨�����д�������ֹ��\n");
	//		else printf("�﷨�����ɹ���\n");
	//	}
	//	return 0;
	//}

	LLparse parse;
	vector<string> v({ "id","+","id","*","(","id","+","id",")" });
	//vector<string> v({ "id","+","id","*","id","$"});
	parse.make_tree = true;
	parse.input_file = "production.txt";
	parse.parse(v);
	cout << "�﷨��Ϊ:" << endl;
	parse.grammer_tree->Print(0);
	cout << parse.line << endl;


}