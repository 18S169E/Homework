//#pragma once
//#include "正则_NFA.h"
//#include "NFA_DFA.h"
//#include "基于自动机的词法分析器.h"
//
//int main() {
////	string s = "(a|b)*abb";
////	string s2 = "10|(0|11)0*";
//	string s3 = "";
////初始化states(NFA)
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
//	cout << "初始值为:" << start << endl;
//	print_DFA(d, s3);
//
//	//match_machine();
//
//	return 0;
//}