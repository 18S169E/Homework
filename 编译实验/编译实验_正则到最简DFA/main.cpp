//#pragma once
//#include "����_NFA.h"
//#include "NFA_DFA.h"
//#include "�����Զ����Ĵʷ�������.h"
//
//int main() {
////	string s = "(a|b)*abb";
////	string s2 = "10|(0|11)0*";
//	string s3 = "";
////��ʼ��states(NFA)
//	for (int i = 0; i < MAXN; i++) {
//		states[i].index = i;
//		states[i].c = epsilon;
//		states[i].to = -1;
//	}
//
//	//s3 = s3 + LETTER + "(" + LETTER + "|" + DIGIT + ")*";
//	s3 = s3 + DIGIT + "(" + LETTER  + ")";
//	NFA n = re2nfa(s3);
//	int start = 0;
//	map<pair<int, char>, int> d = NFA2DFA(n, s3, start);
//	cout << "��ʼֵΪ:" << start << endl;
//	print_DFA(d, s3);
//
//	//match_machine();
//
//	return 0;
//}