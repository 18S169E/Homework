#pragma once
#include "正则_NFA.h"
#include "NFA_DFA.h"

int main() {
	string s = "(a|b)*abb";
	//string s = "10|(0|11)0*";
//初始化states(NFA)
	for (int i = 0; i < MAXN; i++) {
		states[i].index = i;
		states[i].c = epsilon;
		states[i].to = -1;
	}
	NFA n = re2nfa(s);

	map<pair<int, char>, int> d = NFA2DFA(n, s);


	return 0;
}