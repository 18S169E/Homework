//#include "re2nfa.h"
//
//
//void test(string s) {
//	NFA n = re2NFA(s);
//	cout << "NFA:" << endl <<  n << endl;
//	DFA d = NFA2DFA(n);
//	cout << "DFA:" << endl << d << endl;
//}
//
//int main() {
//	string DIGIT = "(1|2|3|4|5|6|7|8|9|0)";
//	//string LETTER = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
//	//string s3 = "@(@|#)*";
//	string s3= "##";
//	//NFA n = re2NFA(DIGIT);
//	//cout << "NFA"<<n << endl;
//	//DFA d = NFA2DFA(n);
//	//DFA d3 = re2DFA(s3);
//	//cout << "d1:" << endl << d << endl;
//	//cout << "d3:" << endl<< d3 << endl;
//	
//	
//	string tot[] = { DIGIT,s3 };
//	for (auto s : tot) {
//		test(s);
//	}
//
//	return 0;
//
//}