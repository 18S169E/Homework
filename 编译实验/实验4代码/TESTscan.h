
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


//操作符 * | &
//操作数 字母 数字 +-×{}:,;/<>!@#（）=


//*       -> ×
//()      -> （）

//DEBUG模式
#define DEBUG 0
//利用类似四则混合运算的思想，分为操作符和操作数去计算，所以需要添加与操作符，同时，将中缀改为后缀
//有一个缺陷,那就是初态不一样是0,终态不一定是NFA_NUM

#define epsilon '#'

#define START 0

#define re2DFA(s) NFA2DFA(re2NFA(s))
//符号表
set<char> symbols;

//操作数
const string valid = "+-×{}:,;/<>!（）=";
bool ifvalid(char x) {
	for (auto v : valid) if (x == v) return true;
	return isalnum(x);
};

//获取可能的输入符号
set<char> get_sign(string s) {
	set<char> ans;
	for (auto x : s) {
		if (ifvalid(x)) ans.insert(x);
	}
	return ans;
}

//由后缀表达式构造NFA,其中空串用#表示
//NFA结构：=由节点构成，节点用数字表示  每个实例记录开始节点和最终节点，且用类变量存放节点与节点之间的转换关系

//在字符串中添加与（&）操作符  并且获取符号表
void preprocess(string& s) {
	//对于字符xy,需要x为操作数，),*   ;   y为操作数，（  中添加操作符
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

//中缀转为后缀  并检查是否存在非法字符和括号是否匹配
string in2post(const string& s) {
	//返回操作符的优先级
	map<char, int> prio = {
		{'&',1},
		{'|',2},
		{'*',3},
	};

	//存放操作符
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
					printf("括号不匹配");
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
				printf("非法操作符");
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
	//成员
		//开始节点和中止节点
	int start;
	int end;

	//状态转换图 和当前符号
	static map<pair<int, char>, int> Tran;   //Tran[{a,char}] = b
	static map<int, set<int>> eps;           //eps[a] = {b,c,d...}
	static int cur;
	static void add(int a, int b, char c = '#');

	//函数
		//构造函数
	NFA() {};
	NFA(int start, int end) :start(start), end(end) {};

	// 自定义输出操作符
	friend std::ostream& operator<<(std::ostream& os, const NFA& nfa) {
		os << "Start: " << nfa.start << ", End: " << nfa.end << "\n";
		return os;
	}
	//输出转换图
	void print_Tran() {
		cout << "NFA转换图:" << endl;
		//单纯打印所有元素，不要求有序
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

// 在类外部进行定义和初始化
map<std::pair<int, char>, int> NFA::Tran;
map<int, set<int>> NFA::eps;
int NFA::cur = START;
void NFA::add(int a, int b, char c) {
	if (c != '#') {
		Tran[{a, c}] = b;
	}
	//空转换
	else {
		eps[a].insert(b);
	}
}

NFA post2NFA(const string& s) {
#define CUR (NFA::cur)
#define NEXT (CUR+1)
	stack<NFA> NFAS;
	for (const auto& x : s) {
		//操作符
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


//正则到NFA
NFA re2NFA(string s) {

	preprocess(s);
	string s_post = in2post(s);
	NFA n = post2NFA(s_post);

	if (DEBUG) {
		printf("DEBUG:pre:%s\n", s.c_str());
		printf("DEBUG:post:%s\n", s_post.c_str());
		//printf("DEBUG:NFA状态图\n");
		//cout << n << endl;
		//n.print_Tran();
	}

	return n;
}


struct DFA {
public:
	//成员
		//开始节点和中止节点
	int start;
	set<int> states; // 对应NFA的编号
	set<int> end;  //结尾

	//状态转换图 和当前符号
	static map<pair<int, char>, int> Tran;   //Tran[{a,char}] = b
	static int cur;
	static void add(int a, int b, char c);

	//函数
		//构造函数
	DFA() {};
	DFA(int start) :start(start), states({}), end({}) {};
	DFA(int start, set<int> states) :start(start), states(states), end({}) {};

	// 自定义输出操作符
	friend std::ostream& operator<<(std::ostream& os, const DFA& dfa) {
		os << "Start: " << dfa.start << ", End: ";

		for (auto x : dfa.end) {
			cout << x << " ";
		}
		cout << endl;
		return os;
	}
	//输出转换图
	void print_Tran() {
		//单纯打印所有元素，不要求有序
		cout << "DFA转换图:" << endl;

		for (const auto& x : this->Tran) {
			int a = x.first.first;
			char c = x.first.second;
			int b = x.second;
			cout << a << " " << c << " " << b << endl;
		}
	}
};

// 在类外部进行定义和初始化
map<std::pair<int, char>, int> DFA::Tran;
int DFA::cur = START;
void DFA::add(int a, int b, char c) {
	Tran[{a, c}] = b;
}

//map<int, set<int>> store;
//闭包函数 set<int>  -> set<int>
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
	//根据NFA状态分别DFA
	map<set<int>, int> m;//NFA状态转换到DFA 
	//map<set<int>, bool> flag;//是否标记
	DFA s0 = DFA(CUR, clo_s0);
	m[clo_s0] = CUR;
	CUR++;
	for (auto k : clo_s0) {
		if (k == n.end) s0.end.insert(m[clo_s0]);
	}

	queue<set<int>> Dstates;
	Dstates.push(clo_s0);
	//只存未标记的状态
	while (!Dstates.empty()) {

		set<int> T = Dstates.front();
		Dstates.pop();
		//标记T
		//if (flag.find(T) == flag.end()) flag[T] = true;
		//else continue;
		//cnt++;
		for (auto a : symbols) {
			set<int> clo_U = closure(move(T, a));
			//为空则跳过
			if (clo_U.size() == 0) {
				continue;
			}
			//clo_U没有对应的DFA  那么就要新建一个
			if (m.find(clo_U) == m.end()) {
				//DFA U(CUR, clo_U);
				m[clo_U] = CUR;
				CUR++;
				//判断U中是否有终结节点
				if (clo_U.find(n.end) != clo_U.end())s0.end.insert(CUR - 1);
				Dstates.push(clo_U);
			}
			//问题是太多的节点转换到-1节点
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
	//输出转换图
public:
	MINDFA() {};
	void print_Tran() {
		//单纯打印所有元素，不要求有序
		cout << "MINDFA转换图为:" << endl;
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
	//自己的实现
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
					//把大的改为小的，且若小的改过了则改为更小的
					if (change.find(*s) != change.end()) change[*t] = change[*s];
					else change[*t] = *s;
				}
			}
		}
	}
	//运用change将原来DFA的值缩小
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

	//组装进新的MIN——DFA
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

	//cout << "字符串" << s << endl;
	NFA n = re2NFA(s);
	DFA d = NFA2DFA(n);
	MINDFA m = DFA2MIN(d);


	//m.print_Tran();


	if (TEST) {
		cout << "NFA:\n转换图大小为:" << n.Tran.size() + n.eps.size() << "\n" << n << endl;
		//n.print_Tran();
		cout << "DFA:\n转换图大小为:" << d.Tran.size() << "\n" << d << endl;
		//d.print_Tran();

	}

	return m;
}


#include <fstream>


#define keywordSum 8
const char* keyword[keywordSum] = { "if", "else", "for", "while", "do", "int", "read", "write" };
//下面定义纯单分界符
char singleword[50] = "+-*(){}; ,:";
//下面定义双分界符的首字符
char doubleword[10] = "><=!";


class Rule {
public:
	string name;
	string re;
	MINDFA m;

	//构造函数
	Rule() {};
	Rule(string name, string re) :name(name), re(re) {};
};

class Machine {
public:
	Rule Identifier;
	Rule Num;
	FILE* sourceFile;  // 输入文件流
	FILE* destinationFile; // 输出文件流
	int line; // 行数
	set<int>cur;//当前状态


		//初始化
	void init(string in, string out) {
		//将*替换为×  需要将字符转换为合法字符，且合法字符集要修改。
		//必要的话最后的输出要转回元字符
		string DIGIT = "(1|2|3|4|5|6|7|8|9|0)";
		string LETTER = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";
		this->Num = Rule("num", DIGIT + DIGIT + "*");
		this->Identifier = Rule("identifier", LETTER + "(" + LETTER + "|" + DIGIT + ")*");
		//this->Identifier = Rule("identifier", LETTER);
		Num.m = re2MIN(Num.re);
		Identifier.m = re2MIN(Identifier.re);
		//cout << "ok" << endl;

		//初始化其他成员
		this->line = 1;
		this->sourceFile = fopen(in.c_str(), "r");  // 打开源文件以供读取
		this->destinationFile = fopen(out.c_str(), "w");  // 打开目标文件以供写入

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

		char word[100];//记录当前字符串
		char ch = getc(sourceFile);//记录当前字符
		while (ch != EOF) {  // 逐个字符读取，直到文件结束
			//根据不同字符分别处理
			//空白字符，忽略，读取下一个字符
			while (isspace(ch)) {
				if (ch == '\n')  line++;
				ch = getc(sourceFile);
			}
			//换行符
			if (ch == EOF) return 0;
			//字母  自动机
			if (isalpha(ch)) {
				int index = 0;
				cur.insert(Identifier.m.start);
				do {
					word[index++] = ch;
					ch = getc(sourceFile);
				} while (Iden(ch));
				cur.clear();
				word[index] = '\0';
				//判断是否为保留字
				//printf("DEBUG:word:%s", word);
				bool has_print = 0;
				for (int i = 0; i < keywordSum; i++) {
					//printf("DEBUG:n:%d", i);
					if (strcmp(word, keyword[i]) == 0) {
						fprintf(destinationFile, "%s\t%s\n", word, word);
						has_print = 1;
						//变量名必须以字母开头
						if (strcmp(word, "int") == 0) {
							ch = getc(sourceFile);
							index = 0;
							do {
								word[index++] = ch;
								ch = getc(sourceFile);
							} while (isalnum(ch));
							word[index] = '\0';

							if (!isalpha(word[0])) {
								printf("出错在%d行\n", line);
								printf("变量名不合法\n");
								return 3;
							}
							else fprintf(destinationFile, "%s\t%s\n", word, word);
						}
					}
				}
				//不是保留字
				if (!has_print) fprintf(destinationFile, "%s\t%s\n", "ID", word);
			}
			//数字  自动机
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
			//纯单分节符
			else if (strchr(singleword, ch) > 0) {
				word[0] = ch;
				word[1] = '\0';
				ch = getc(sourceFile);
				fprintf(destinationFile, "%s\t%s\n", word, word);
			}
			//非纯单分节符
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
			//  /号
			else if (ch == '/') {
				ch = getc(sourceFile);
				//注释
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
				printf("出错在%d行\n", line);
				printf("出错字符是%c\n", ch);
				return 3;
			}
		}
		return 0;
	}


	//析构函数
	~Machine() {
		fclose(sourceFile);  // 关闭文件
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