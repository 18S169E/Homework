#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include<iostream>
#include<string>
#include<vector>
#include<stack>
#include<map>
#include<set>
#include<queue>
#include <regex>
using namespace std;

//操作符 * | &
//操作数 字母 数字 +-×{}:,;/<>!@#（）=

//DIGIT   -> #  
//LETTER  -> @
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
const string valid = "+-×{}:,;/<>!@#（）=";
bool ifvalid(char x) {
	for (auto v : valid) if (x == v) return true;
	return isalnum(x);
};

//获取可能的输入符号
set<char> get_sign(string s) {
	set<char> ans;
	std::regex pattern1("#");
	std::regex pattern2("@");
	s = regex_replace(s, pattern1, "(1|2|3|4|5|6|7|8|9|0)");
	s = regex_replace(s, pattern2, "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)");
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
		//单纯打印所有元素，不要求有序
		for (const auto &x : this->Tran) {
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

//DIGIT   -> #  
//LETTER  -> @
NFA Digit;
NFA Letter;

NFA post2NFA(const string& s) {
#define CUR (NFA::cur)
#define NEXT (CUR+1)
	stack<NFA> NFAS;
	for (const auto &x : s) {
		//操作符
		if (ifvalid(x)) {
			if (x == '#') {
				NFAS.push(Digit);
			}
			else if (x == '@') {
				NFAS.push(Letter);
			}
			else {
				NFA n(CUR, NEXT);
				NFA::add(CUR, NEXT, x);
				NFAS.push(n);
				CUR += 2;
			}
		}
		else if (x == '&') {
			NFA n2 = NFAS.top();
			NFAS.pop();
			NFA n1 = NFAS.top();
			NFAS.pop();
			NFA n = NFA(n1.start,n2.end);
			NFA::add(n1.end, n2.start);
			NFAS.push(n);
		}
		else if (x == '|') {
			NFA n2 = NFAS.top();
			NFAS.pop();
			NFA n1 = NFAS.top();
			NFAS.pop();
			NFA n = NFA(CUR,NEXT);
			CUR += 2;
			NFA::add(n.start, n1.start);
			NFA::add(n.start, n2.start);
			NFA::add(n1.end, n.end);
			NFA::add(n2.end, n.end);
			NFAS.push(n);
		}
		else {
			NFA n = NFA(CUR,NEXT);
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
	int type =0;
	if (s == "(1|2|3|4|5|6|7|8|9|0)") {
		type = 1;
	}
	else if (s == "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)")
	{
		type = 2;
	}
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
	if (type == 1) {
		Digit = n;
	}
	else if (type == 2) {
		Letter = n;
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
	DFA(int start,set<int> states) :start(start), states(states), end({}) {};

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
void DFA::add(int a, int b,char c) {
	Tran[{a, c}] = b;
}

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
	map<set<int>, bool> flag;//是否标记
	DFA s0 = DFA(CUR, clo_s0);
	m[clo_s0] = CUR;
	CUR++;
	for (auto k : clo_s0) {
		if (k == n.end) s0.end.insert(m[clo_s0]);
	}
	
	//插入结束状态，也就是空
	m[{}] = -1;
	flag[{}] = true;
	//m[clo_s0] = 0;
	queue<set<int>> Dstates;
	Dstates.push(clo_s0);
	while (!Dstates.empty()) {
		set<int> T = Dstates.front();
		Dstates.pop();
		//标记T
		if (flag.find(T) == flag.end()) flag[T] = true;
		else continue;

		for (auto a : symbols) {
			set<int> clo_U = closure(move(T, a));
			//clo_U没有对应的DFA  那么就要新建一个
			if (m.find(clo_U) == m.end()) {
				DFA U(CUR, clo_U);
				m[clo_U] = CUR;
				CUR++;
				//判断U中是否有终结节点
				for (auto k : clo_U) {
					if (k == n.end) s0.end.insert(m[clo_U]);
				}
				Dstates.push(clo_U);
			}
			DFA::Tran[{m[T], a}] = m[clo_U];
		}
	}
	
	if (DEBUG) {
		cout << s0 << endl;
		cout << "DFA状态图" << endl;
		s0.print_Tran();
	}

	return s0;
}

void MINDFA(DFA d) {
	set<int> acc, uacc;
	for (int i = d.start; i < DFA::cur; i++) {
		if (d.end.find(i) != d.end.end()) acc.insert(i);
		else uacc.insert(i);
	}
	//存放所有划分
	vector<set<int>> splits;
	while (1) {
		//修改到直到没变
		vector<set<int>> new_splits;
		bool change = false;
		for (auto split : splits) {
			//一个划分内有多个状态

		}
	}
		 
}