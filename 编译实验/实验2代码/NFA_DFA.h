#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "正则_NFA.h"
#include <queue>
#include <map>
#define DEBUG 1

int DFA_num = 0;

class DFA {
public:
	set<int> mems;
	bool end;
	int id;

	DFA() {
		this->id = -1;
	};

	DFA(set<int>mems, bool end, int id = -1) {
		this->mems = mems;
		this->end = end;
		this->id = id;
	}
	bool operator<(const DFA other) const {
		auto it1 = this->mems.begin();
		auto it2 = other.mems.begin();
		while (it1 != this->mems.end() && it2 != other.mems.end()) {
			if (*it1 < *it2) return true;
			it1++;
			it2++;
		}
		return false;
	}

	bool operator==(const DFA other) const {
		return this->mems == other.mems;
	}

	void print() {
		if (this->id == -1) return;
		printf("id:%d\tmems:",this->id);
		for (auto mem : mems) {
			cout << mem << " ";
		}
		printf("\n");
	}
};

//判断DFA是否被标记
bool if_in(const set<DFA>& flags, const DFA& d) {
	for (auto x : flags) {
		if (x == d) return true;
	}
	return false;
}

//move
set<int> move(DFA d, char a) {
	set<int> ans;
	for (auto x : d.mems) {
		if (states[x].c == a) ans.insert(states[x].to);
	}
	return ans;
}

//闭包函数
set<int> closure(set<int> T){
	set<int> ans = T;
	stack<int> s;
	for (auto &t : T) {
		s.push(t);
	}
	while (!s.empty()) {
		auto x = s.top();
		s.pop();
		for (auto ep : states[x].eps) {
			if (ans.find(ep) == ans.end()) {
				ans.insert(ep);
				s.push(ep);
			}
		}

	}
	return ans;
}

//获取可能的输入符号
set<char> get_sign(const string& s) {
	set<char> ans;
	for (auto x : s) {
		if(isalnum(x)) ans.insert(x);
	}
	return ans;
}

//closure 是否为中止DFA
bool if_end(set<int> s, NFA n) {
	for (auto x : s) {
		if (x == n.tail->index) return true;
	}
	return false;
}

//用数组记录 DFA
DFA DFAS[MAXN];
void print_DFA(map<pair<int, char>, int> Dtran, const string& s);
void minimize(map<pair<int, char>, int>& Dtran, string s);
//NFA2DFA
map<pair<int, char>, int> NFA2DFA(NFA n, const string &s) {

	map<pair<int,char>, int> Dtran;
	queue<DFA> Dstates;

	DFA D0 = DFA(closure(set<int>{n.head->index}), 0, DFA_num++);
	Dstates.push(D0);
	DFAS[0] = D0;
	set<char> signs = get_sign(s);
	//标记
	set<DFA> flags;
	while (true) {
		if (Dstates.empty()) break;
		DFA T = Dstates.front();
		Dstates.pop();
		if (if_in(flags, T)) continue;
		flags.insert(T);
		//每个输入符号
		for (auto sign : signs) {
			auto clo = closure(move(T, sign));
			if (clo.size() == 0) continue;
			DFA U = DFA(clo, if_end(clo, n));
			//printf("T.id:%d\t%c\t\n", T.id, sign);
			//for (auto mem : U.mems) {
			//	cout << mem << ' ';
			//}
			//cout << endl;
			if (!if_in(flags, U)) {
				U.id = DFA_num++;
				//printf("DEBUG:U.id:%d\tDFA_num:%d\n", U.id, DFA_num);
				DFAS[DFA_num - 1] = U;
				Dstates.push(U);
			}
			else {
				U.id = flags.find(U)->id;
			}
			Dtran[make_pair(T.id, sign)] = U.id;
		}

	}
	if (DEBUG) {
		print_DFA(Dtran, s);
	}
	minimize(Dtran, s);
	return Dtran;
}

//print DFA
void print_DFA(map<pair<int, char>, int> Dtran,const string &s) {
	printf("\nDFA:\n");
	set<int> ends;
	printf("\t状态\t输入符号\n");
	printf("\t\t");
	
	set<char> signs = get_sign(s);
	for (auto x : signs) {
		printf("%c\t", x);
	}
	printf("\n");
	for (auto x : DFAS) {
		if (x.id != -1) {
			printf("\t%d\t",x.id);
			for (auto sign : signs) {
				if (Dtran.find(make_pair(x.id, sign)) == Dtran.end()) {
					printf("\t");
				}
				else {
					printf("%d\t", Dtran[make_pair(x.id, sign)]);
				}
			}
			printf("\n");
			if (x.end) ends.insert(x.id);
		}
	}
	printf("终态:\n");
	for (auto end : ends) {
		cout << end << ' ';
	}
	cout << endl;
}

class MIN_DFA {
public:
	int id;
	set<pair<char, int>> trans;
	bool end;
};
void print_MINDFA(vector<set<int>> split);
void minimize(map<pair<int, char>, int> &Dtran,string s) {

	set<char> signs = get_sign(s);
	vector<set<int>> split;
	set<int> acc;
	set<int> uacc;
	for (auto x : DFAS) {
		if (x.id != -1) {
			if (x.end) acc.insert(x.id);
			else uacc.insert(x.id);
		}
	}
	split.push_back(acc);
	split.push_back(uacc);
	while (1) {
		vector<set<int>> split_new;
		int visit[MAXN];
		for (int i = 0; i < MAXN; i++) {
			visit[i] = 0;
		}
		for (auto G : split) {
			
			for (auto s = G.begin(); s != G.end(); s++) {
				
				if (visit[*s]) continue;
				//新子集
				set<int> son;
				son.insert(*s);
				//printf("DEBUG:im here\n");
				for (auto t = next(s); t != G.end(); t++) {
					if (visit[*t]) continue;
					bool same = 1;
					for (auto sign : signs) {
						auto temp1 = make_pair(*s, sign);
						auto temp2 = make_pair(*t, sign);
						if (Dtran.find(temp1) != Dtran.end() && Dtran.find(temp2) !=
							Dtran.end() && Dtran[temp1] == Dtran[temp2]) {
							same = 1;
						}
						else same = 0;
					}
					if (same) {
						son.insert(*t);
						visit[*t] = 1;
					}
				}
				split_new.push_back(son);
			}
		}
		if (split_new == split) {
			print_MINDFA(split);
			break;
		}
		else {
			split = split_new;
		}
	}

}

void print_MINDFA(vector<set<int>> split) {
	cout << "MINDFA" << endl;

	for (auto x : split) {
		cout << "状态:	";
		for (auto y : x) {
			cout << y << " ";
		}
		cout << endl;
	}
	cout << endl;
}
