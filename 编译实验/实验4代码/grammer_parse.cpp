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
		if (es > 0) printf("�ʷ������д�������ֹ��\n");
		else printf("�ʷ������ɹ���\n");
		if (es == 0)
		{
			es = TESTparse();
			if (es > 0) printf("�﷨�����д�������ֹ��\n");
			else printf("�﷨�����ɹ���\n");
		}
		return 0;
	}
	else {

		int es = 0;
		printf("�ʷ������ɹ���\n");
		if (es == 0)
		{
			es = TESTparse();
			if (es > 0) printf("�﷨�����д�������ֹ��\n");
			else printf("�﷨�����ɹ���\n");
		}
		return 0;
	}




}