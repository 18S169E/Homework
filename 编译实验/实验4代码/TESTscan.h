
#define _CRT_SECURE_NO_WARNINGS

#pragma once
#include "config.h"
#include<iostream>
#include<string>
#include<vector>
#include<stack>
#include<map>
#include<set>
#include<queue>
#include <string.h>
using namespace std;


//������ * | &
//������ ��ĸ ���� +-��{}:,;/<>!@#����=


//*       -> ��
//()      -> ����

//DEBUGģʽ
#define DEBUG 0
//�������������������˼�룬��Ϊ�������Ͳ�����ȥ���㣬������Ҫ������������ͬʱ������׺��Ϊ��׺
//��һ��ȱ��,�Ǿ��ǳ�̬��һ����0,��̬��һ����NFA_NUM

#define epsilon '#'

#define START 0

#define re2DFA(s) NFA2DFA(re2NFA(s))
//���ű�
set<char> symbols;

//������
const string valid = "+-��{}:,;/<>!����=";
bool ifvalid(char x) {
	for (auto v : valid) if (x == v) return true;
	return isalnum(x);
};

//��ȡ���ܵ��������
set<char> get_sign(string s) {
	set<char> ans;
	for (auto x : s) {
		if (ifvalid(x)) ans.insert(x);
	}
	return ans;
}

//�ɺ�׺���ʽ����NFA,���пմ���#��ʾ
//NFA�ṹ��=�ɽڵ㹹�ɣ��ڵ������ֱ�ʾ  ÿ��ʵ����¼��ʼ�ڵ�����սڵ㣬�����������Žڵ���ڵ�֮���ת����ϵ

//���ַ���������루&��������  ���һ�ȡ���ű�
void preprocess(string& s) {
	//�����ַ�xy,��ҪxΪ��������),*   ;   yΪ����������  ����Ӳ�����
	for (int i = 0; i < s.size() - 1; i++) {
		auto& x = s[i];
		auto& y = s[i + 1];
		if (ifvalid(x) || x == ')' || x == '*') {
			if (ifvalid(y) || y == '(') {
				s.insert(i + 1, "&");
				i++;
			}
		}
	}

	symbols = get_sign(s);
}

//��׺תΪ��׺  ������Ƿ���ڷǷ��ַ��������Ƿ�ƥ��
string in2post(const string& s) {
	//���ز����������ȼ�
	map<char, int> prio = {
		{'&',1},
		{'|',2},
		{'*',3},
	};

	//��Ų�����
	stack<char> ops;
	string ans = "";
	for (auto& x : s) {
		if (ifvalid(x)) {
			ans += x;
		}
		else if (x == '(') {
			ops.push(x);
		}
		else if (x == ')') {
			char temp;
			while (1) {
				if (ops.empty()) {
					printf("���Ų�ƥ��");
					exit(EXIT_FAILURE);
				}
				temp = ops.top();
				ops.pop();
				if (temp != '(') {
					ans += temp;
				}
				else break;
			}
		}
		else {
			bool match = 0;
			if (x == '|' || x == '&' || x == '*') {
				while (1) {
					if (!ops.empty()) {
						char temp = ops.top();
						if (prio[temp] >= prio[x]) {
							ans += temp;
							ops.pop();
						}
						else break;
					}
					else break;
				}
				ops.push(x);
			}
			else {
				printf("�Ƿ�������");
				exit(EXIT_FAILURE);
			}
		}
	}
	while (!ops.empty()) {
		char temp = ops.top();
		ops.pop();
		ans += temp;
	}
	return ans;
}

struct NFA {
public:
	//��Ա
		//��ʼ�ڵ����ֹ�ڵ�
	int start;
	int end;

	//״̬ת��ͼ �͵�ǰ����
	static map<pair<int, char>, int> Tran;   //Tran[{a,char}] = b
	static map<int, set<int>> eps;           //eps[a] = {b,c,d...}
	static int cur;
	static void add(int a, int b, char c = '#');

	//����
		//���캯��
	NFA() {};
	NFA(int start, int end) :start(start), end(end) {};

	// �Զ������������
	friend std::ostream& operator<<(std::ostream& os, const NFA& nfa) {
		os << "Start: " << nfa.start << ", End: " << nfa.end << "\n";
		return os;
	}
	//���ת��ͼ
	void print_Tran() {
		cout << "NFAת��ͼ:" << endl;
		//������ӡ����Ԫ�أ���Ҫ������
		for (const auto& x : this->Tran) {
			int a = x.first.first;
			char c = x.first.second;
			int b = x.second;
			cout << a << " " << c << " " << b << endl;
		}
		for (auto y : NFA::eps) {
			for (auto z : y.second) {
				cout << y.first << "   " << z << endl;
			}
		}
	}
};

// �����ⲿ���ж���ͳ�ʼ��
map<std::pair<int, char>, int> NFA::Tran;
map<int, set<int>> NFA::eps;
int NFA::cur = START;
void NFA::add(int a, int b, char c) {
	if (c != '#') {
		Tran[{a, c}] = b;
	}
	//��ת��
	else {
		eps[a].insert(b);
	}
}

NFA post2NFA(const string& s) {
#define CUR (NFA::cur)
#define NEXT (CUR+1)
	stack<NFA> NFAS;
	for (const auto& x : s) {
		//������
		if (ifvalid(x)) {
			NFA n(CUR, NEXT);
			NFA::add(CUR, NEXT, x);
			NFAS.push(n);
			CUR += 2;
		}
		else if (x == '&') {
			NFA n2 = NFAS.top();
			NFAS.pop();
			NFA n1 = NFAS.top();
			NFAS.pop();
			NFA n = NFA(n1.start, n2.end);
			NFA::add(n1.end, n2.start);
			NFAS.push(n);
		}
		else if (x == '|') {
			NFA n2 = NFAS.top();
			NFAS.pop();
			NFA n1 = NFAS.top();
			NFAS.pop();
			NFA n = NFA(CUR, NEXT);
			CUR += 2;
			NFA::add(n.start, n1.start);
			NFA::add(n.start, n2.start);
			NFA::add(n1.end, n.end);
			NFA::add(n2.end, n.end);
			NFAS.push(n);
		}
		else {
			NFA n = NFA(CUR, NEXT);
			CUR += 2;
			NFA n1 = NFAS.top();
			NFAS.pop();
			NFA::add(n1.end, n1.start);
			NFA::add(n.start, n1.start);
			NFA::add(n1.end, n.end);
			NFA::add(n.start, n.end);
			NFAS.push(n);
		}
	}
	return NFAS.top();

}


//����NFA
NFA re2NFA(string s) {

	preprocess(s);
	string s_post = in2post(s);
	NFA n = post2NFA(s_post);

	if (DEBUG) {
		printf("DEBUG:pre:%s\n", s.c_str());
		printf("DEBUG:post:%s\n", s_post.c_str());
		//printf("DEBUG:NFA״̬ͼ\n");
		//cout << n << endl;
		//n.print_Tran();
	}

	return n;
}


struct DFA {
public:
	//��Ա
		//��ʼ�ڵ����ֹ�ڵ�
	int start;
	set<int> states; // ��ӦNFA�ı��
	set<int> end;  //��β

	//״̬ת��ͼ �͵�ǰ����
	static map<pair<int, char>, int> Tran;   //Tran[{a,char}] = b
	static int cur;
	static void add(int a, int b, char c);

	//����
		//���캯��
	DFA() {};
	DFA(int start) :start(start), states({}), end({}) {};
	DFA(int start, set<int> states) :start(start), states(states), end({}) {};

	// �Զ������������
	friend std::ostream& operator<<(std::ostream& os, const DFA& dfa) {
		os << "Start: " << dfa.start << ", End: ";

		for (auto x : dfa.end) {
			cout << x << " ";
		}
		cout << endl;
		return os;
	}
	//���ת��ͼ
	void print_Tran() {
		//������ӡ����Ԫ�أ���Ҫ������
		cout << "DFAת��ͼ:" << endl;

		for (const auto& x : this->Tran) {
			int a = x.first.first;
			char c = x.first.second;
			int b = x.second;
			cout << a << " " << c << " " << b << endl;
		}
	}
};

// �����ⲿ���ж���ͳ�ʼ��
map<std::pair<int, char>, int> DFA::Tran;
int DFA::cur = START;
void DFA::add(int a, int b, char c) {
	Tran[{a, c}] = b;
}

//map<int, set<int>> store;
//�հ����� set<int>  -> set<int>
set<int> closure(set<int> T) {
	set<int> ans = T;
	stack<int> s;
	for (auto& t : T) {
		s.push(t);
	}
	while (!s.empty()) {
		auto x = s.top();
		s.pop();
		for (auto ep : NFA::eps[x]) {
			ans.insert(ep);
			s.push(ep);
		}
	}
	return ans;
}

//move  set<int>  -> set<int>
set<int> move(set<int> s, char c) {
	set<int> ans;
	for (auto x : s) {
		if (NFA::Tran.find({ x, c }) != NFA::Tran.end()) {
			ans.insert(NFA::Tran[{x, c}]);
		}
	}
	return ans;
}


DFA NFA2DFA(NFA n) {
#define CUR (DFA::cur)
	set<int> clo_s0 = closure({ n.start });
	//����NFA״̬�ֱ�DFA
	map<set<int>, int> m;//NFA״̬ת����DFA 
	//map<set<int>, bool> flag;//�Ƿ���
	DFA s0 = DFA(CUR, clo_s0);
	m[clo_s0] = CUR;
	CUR++;
	for (auto k : clo_s0) {
		if (k == n.end) s0.end.insert(m[clo_s0]);
	}

	queue<set<int>> Dstates;
	Dstates.push(clo_s0);
	//ֻ��δ��ǵ�״̬
	while (!Dstates.empty()) {

		set<int> T = Dstates.front();
		Dstates.pop();
		//���T
		//if (flag.find(T) == flag.end()) flag[T] = true;
		//else continue;
		//cnt++;
		for (auto a : symbols) {
			set<int> clo_U = closure(move(T, a));
			//Ϊ��������
			if (clo_U.size() == 0) {
				continue;
			}
			//clo_Uû�ж�Ӧ��DFA  ��ô��Ҫ�½�һ��
			if (m.find(clo_U) == m.end()) {
				//DFA U(CUR, clo_U);
				m[clo_U] = CUR;
				CUR++;
				//�ж�U���Ƿ����ս�ڵ�
				if (clo_U.find(n.end) != clo_U.end())s0.end.insert(CUR - 1);
				Dstates.push(clo_U);
			}
			//������̫��Ľڵ�ת����-1�ڵ�
			DFA::Tran[{m[T], a}] = m[clo_U];
		}
	}

	return s0;
}

////2.0
//DFA NFA2DFA(NFA n) {
//
//}

class MINDFA {
public:
	int start;
	set<int> end;
	map<std::pair<int, char>, int> Tran;
	MINDFA(int start, set<int> end, map<std::pair<int, char>, int> Tran) :start(start), end(end), Tran(Tran) {};
	//���ת��ͼ
public:
	MINDFA() {};
	void print_Tran() {
		//������ӡ����Ԫ�أ���Ҫ������
		cout << "MINDFAת��ͼΪ:" << endl;
		for (const auto& x : this->Tran) {
			int a = x.first.first;
			char c = x.first.second;
			int b = x.second;
			cout << a << " " << c << " " << b << endl;
		}
	}
	friend std::ostream& operator<<(std::ostream& os, const MINDFA& dfa) {
		os << "Start: " << dfa.start << ", End: ";

		for (auto x : dfa.end) {
			cout << x << " ";
		}
		cout << endl;
		return os;
	}

};

MINDFA DFA2MIN(DFA d) {
	map<std::pair<int, char>, int> ans;
	DFA start = d;
	//DFA::Tran = ans;
	set<int> acc, uacc;
	for (int i = d.start; i < DFA::cur; i++) {
		if (d.end.find(i) != d.end.end()) acc.insert(i);
		else uacc.insert(i);
	}
	vector<set<int>> pai;
	pai.push_back(acc);
	pai.push_back(uacc);
	//�Լ���ʵ��
	map<int, int> change;
	for (auto G : pai) {
		for (auto s = G.begin(); s != G.end(); s++) {
			for (auto t = next(s); t != G.end(); t++) {
				bool same = 1;
				for (auto symbol : symbols) {
					auto temp1 = make_pair(*s, symbol);
					auto temp2 = make_pair(*t, symbol);
					if (DFA::Tran.find(temp1) != DFA::Tran.end() && DFA::Tran.find(temp2) !=
						DFA::Tran.end() && DFA::Tran[temp1] != DFA::Tran[temp2]) {
						same = 0;
					}
					else same = 1;
				}
				if (same) {
					//�Ѵ�ĸ�ΪС�ģ�����С�ĸĹ������Ϊ��С��
					if (change.find(*s) != change.end()) change[*t] = change[*s];
					else change[*t] = *s;
				}
			}
		}
	}
	//����change��ԭ��DFA��ֵ��С
	for (auto it = DFA::Tran.begin(); it != DFA::Tran.end(); it++) {
		int a = it->first.first;
		char c = it->first.second;
		int b = it->second;
		if (change.find(a) != change.end()) {
			a = change[a];
		}
		if (change.find(b) != change.end()) b = change[b];
		ans[{a, c}] = b;
	}

	//��װ���µ�MIN����DFA
	int new_start;
	set<int> new_end;
	if (change.find(d.start) != change.end()) {
		new_start = change[d.start];
	}
	else new_start = d.start;

	for (auto x : d.end) {
		if (change.find(x) != change.end()) {
			new_end.insert(change[x]);
		}
		else new_end.insert(x);
	}
	MINDFA m(new_start, new_end, ans);
	return m;
}

MINDFA re2MIN(string s) {
#define TEST 0

	//cout << "�ַ���" << s << endl;
	NFA n = re2NFA(s);
	DFA d = NFA2DFA(n);
	MINDFA m = DFA2MIN(d);


	//m.print_Tran();


	if (TEST) {
		cout << "NFA:\nת��ͼ��СΪ:" << n.Tran.size() + n.eps.size() << "\n" << n << endl;
		//n.print_Tran();
		cout << "DFA:\nת��ͼ��СΪ:" << d.Tran.size() << "\n" << d << endl;
		//d.print_Tran();

	}

	return m;
}


#include <fstream>


#define keywordSum 8
const char* keyword[keywordSum] = { "if", "else", "for", "while", "do", "int", "read", "write" };
//���涨�崿���ֽ��
char singleword[50] = "+-*(){}; ,:";
//���涨��˫�ֽ�������ַ�
char doubleword[10] = "><=!";


class Rule {
public:
	string name;
	string re;
	MINDFA m;

	//���캯��
	Rule() {};
	Rule(string name, string re) :name(name), re(re) {};
};

class Machine {
public:
	Rule Identifier;
	Rule Num;
	FILE* sourceFile;  // �����ļ���
	FILE* destinationFile; // ����ļ���
	int line; // ����
	set<int>cur;//��ǰ״̬


		//��ʼ��
	void init(string in, string out) {
		//��*�滻Ϊ��  ��Ҫ���ַ�ת��Ϊ�Ϸ��ַ����ҺϷ��ַ���Ҫ�޸ġ�
		//��Ҫ�Ļ��������Ҫת��Ԫ�ַ�
		string DIGIT = "(1|2|3|4|5|6|7|8|9|0)";
		string LETTER = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
		this->Num = Rule("num", DIGIT + DIGIT + "*");
		this->Identifier = Rule("identifier", LETTER + "(" + LETTER + "|" + DIGIT + ")*");
		//this->Identifier = Rule("identifier", LETTER);
		Num.m = re2MIN(Num.re);
		Identifier.m = re2MIN(Identifier.re);
		//cout << "ok" << endl;

		//��ʼ��������Ա
		this->line = 1;
		this->sourceFile = fopen(in.c_str(), "r");  // ��Դ�ļ��Թ���ȡ
		this->destinationFile = fopen(out.c_str(), "w");  // ��Ŀ���ļ��Թ�д��

	}

	bool Iden(char c) {
		set<int> new_;
		for (auto x : cur) {
			if (Identifier.m.Tran.find({ x,c }) != Identifier.m.Tran.end()) {
				new_.insert(Identifier.m.Tran[{ x, c }]);
			}
		}
		if (new_.empty()) {

			cur.clear();
			cur.insert(Num.m.start);
			return false;
		}
		else {
			cur = new_;
			return true;
		}
	}

	bool Digi(char c) {
		set<int> new_;
		for (auto x : cur) {
			if (Num.m.Tran.find({ x,c }) != Num.m.Tran.end()) {
				new_.insert(Num.m.Tran[{ x, c }]);
			}
		}
		if (new_.empty()) {

			cur.clear();
			cur.insert(Num.m.start);
			return false;
		}
		else {
			cur = new_;
			return true;
		}
	}

	int TESTscan()
	{
		if (sourceFile == NULL) return 1;
		if (destinationFile == NULL) return 2;

		char word[100];//��¼��ǰ�ַ���
		char ch = getc(sourceFile);//��¼��ǰ�ַ�
		while (ch != EOF) {  // ����ַ���ȡ��ֱ���ļ�����
			//���ݲ�ͬ�ַ��ֱ���
			//�հ��ַ������ԣ���ȡ��һ���ַ�
			while (isspace(ch)) {
				if (ch == '\n')  line++;
				ch = getc(sourceFile);
			}
			//���з�
			if (ch == EOF) return 0;
			//��ĸ  �Զ���
			if (isalpha(ch)) {
				int index = 0;
				cur.insert(Identifier.m.start);
				do {
					word[index++] = ch;
					ch = getc(sourceFile);
				} while (Iden(ch));
				cur.clear();
				word[index] = '\0';
				//�ж��Ƿ�Ϊ������
				//printf("DEBUG:word:%s", word);
				bool has_print = 0;
				for (int i = 0; i < keywordSum; i++) {
					//printf("DEBUG:n:%d", i);
					if (strcmp(word, keyword[i]) == 0) {
						fprintf(destinationFile, "%s\t%s\n", word, word);
						has_print = 1;
						//��������������ĸ��ͷ
						if (strcmp(word, "int") == 0) {
							ch = getc(sourceFile);
							index = 0;
							do {
								word[index++] = ch;
								ch = getc(sourceFile);
							} while (isalnum(ch));
							word[index] = '\0';

							if (!isalpha(word[0])) {
								printf("������%d��\n", line);
								printf("���������Ϸ�\n");
								return 3;
							}
							else fprintf(destinationFile, "%s\t%s\n", word, word);
						}
					}
				}
				//���Ǳ�����
				if (!has_print) fprintf(destinationFile, "%s\t%s\n", "ID", word);
			}
			//����  �Զ���
			else if (isdigit(ch)) {
				int index = 0;
				cur.insert(Num.m.start);
				do {
					word[index++] = ch;
					ch = getc(sourceFile);
				} while (Digi(ch));
				cur.clear();
				word[index] = '\0';
				fprintf(destinationFile, "%s\t%s\n", "NUM", word);
			}
			//�����ֽڷ�
			else if (strchr(singleword, ch) > 0) {
				word[0] = ch;
				word[1] = '\0';
				ch = getc(sourceFile);
				fprintf(destinationFile, "%s\t%s\n", word, word);
			}
			//�Ǵ����ֽڷ�
			else if (strchr(doubleword, ch) > 0) {
				word[0] = ch;
				ch = getc(sourceFile);
				if (ch == '=') {
					word[1] = ch;
					ch = getc(sourceFile);
					word[2] = '\0';
				}
				else {
					word[1] = '\0';
				}
				fprintf(destinationFile, "%s\t%s\n", word, word);
			}
			//  /��
			else if (ch == '/') {
				ch = getc(sourceFile);
				//ע��
				if (ch == '*') {
					char ch1 = getc(sourceFile);
					do {
						ch = ch1;
						ch1 = getc(sourceFile);
					} while ((ch1 != EOF && (ch != '*' || ch1 != '/')));
					ch = getc(sourceFile);
				}
				else {
					fprintf(destinationFile, "%s\t%s\n", "/", "/");
				}
			}
			else {
				printf("������%d��\n", line);
				printf("�����ַ���%c\n", ch);
				return 3;
			}
		}
		return 0;
	}


	//��������
	~Machine() {
		fclose(sourceFile);  // �ر��ļ�
		fclose(destinationFile);
	}


};



int TESTscan() {
	Machine m;
	char Scanin[300] = "input.txt";
	char Scanout[300] = "output.txt";
	m.init(Scanin, Scanout);
	//string s = "dad32d";
	//for (auto x : s) {
	//	cout << x<<" "<<m.Iden(x) << endl;
	//}
	int es = m.TESTscan();
	return es;
}