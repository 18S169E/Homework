//#pragma once
//#define _CRT_SECURE_NO_WARNINGS
//#include<iostream>
//#include<string>
//#include<vector>
//#include<stack>
//#include<map>
//#include<set>
//using namespace std;
//
////DEBUGģʽ
//#define DEBUG 1
////�������������������˼�룬��Ϊ�������Ͳ�����ȥ���㣬������Ҫ������������ͬʱ������׺��Ϊ��׺
////��һ��ȱ��,�Ǿ��ǳ�̬��һ����0,��̬��һ����NFA_NUM
//
//#define MAXN 128
//#define epsilon '#'
//
////�Ϸ�������
//const string vaild_ops = "*|&";
//
//class NFA_state {
//public:
//	//�ڵ���
//	int index;
//	//Ŀ��ڵ�
//	int to;
//	//�ַ�
//	char c;
//	//��ת��
//	set<int> eps;
//
//};
//
//NFA_state states[MAXN];
//int tot = 0;
//
//class NFA {
//public:
//	NFA_state* head;
//	NFA_state* tail;
//
//	//a->b�ķǿ�ת��
//	static void add(NFA_state* a, NFA_state* b, char x) {
//		a->to = b->index;
//		a->c = x;
//	}
//	//a->b�Ŀ�ת��
//	static void add(NFA_state* a, NFA_state* b) {
//		a->eps.insert(b->index);
//	}
//
//	NFA() {
//		head = &states[tot];
//		tail = &states[tot + 1];
//	}
//
//};
//
//
////���ַ���������루&��������
//void preprocess(string& s) {
//	//�����ַ�xy,��ҪxΪ��������),*   ;   yΪ����������  ����Ӳ�����
//	for (int i = 0; i < s.size() - 1; i++) {
//		auto& x = s[i];
//		auto& y = s[i + 1];
//		if (isalnum(x) || x == ')' || x == '*') {
//			if (isalnum(y) || y == '(') {
//				s.insert(i + 1, "&");
//				i++;
//			}
//		}
//	}
//}
//
////���ز����������ȼ�
//map<char, int> prio = {
//	{'&',1},
//	{'|',2},
//	{'*',3},
//};
//
////��׺תΪ��׺  ������Ƿ���ڷǷ��ַ��������Ƿ�ƥ��
//string in2post(const string& s) {
//	//��Ų�����
//	stack<char> ops;
//	string ans = "";
//	for (auto& x : s) {
//		if (isalnum(x)) {
//			ans += x;
//		}
//		else if (x == '(') {
//			ops.push(x);
//		}
//		else if (x == ')') {
//			char temp;
//			while (1) {
//				if (ops.empty()) {
//					printf("���Ų�ƥ��");
//					exit(EXIT_FAILURE);
//				}
//				temp = ops.top();
//				ops.pop();
//				if (temp != '(') {
//					ans += temp;
//				}
//				else break;
//			}
//		}
//		else {
//			bool match = 0;
//			for (auto& c : vaild_ops) {
//				if (c == x) {
//					match = 1;
//					while (1) {
//						if (!ops.empty()) {
//							char temp = ops.top();
//							if (prio[temp] >= prio[x]) {
//								ans += temp;
//								ops.pop();
//							}
//							else break;
//						}
//						else break;
//					}
//					ops.push(x);
//				}
//			}
//			if (!match) {
//				printf("�Ƿ�������");
//				exit(EXIT_FAILURE);
//			}
//		}
//	}
//	while (!ops.empty()) {
//		char temp = ops.top();
//		ops.pop();
//		ans += temp;
//	}
//	return ans;
//}
//
////��׺תNFA,�ҿ���ѡ���Ƿ��ӡNFA
//NFA post2NFA(const string& s) {
//	stack<NFA> NFAS;
//	for (auto& x : s) {
//		if (isalnum(x)) {
//			NFA n = NFA();
//			tot += 2;
//			NFA::add(n.head, n.tail, x);
//			NFAS.push(n);
//		}
//		else if (x == '&') {
//			NFA n2 = NFAS.top();
//			NFAS.pop();
//			NFA n1 = NFAS.top();
//			NFAS.pop();
//			NFA n = NFA();
//			NFA::add(n1.tail, n2.head);
//			n.head = n1.head;
//			n.tail = n2.tail;
//			NFAS.push(n);
//		}
//		else if (x == '|') {
//			NFA n2 = NFAS.top();
//			NFAS.pop();
//			NFA n1 = NFAS.top();
//			NFAS.pop();
//			NFA n = NFA();
//			tot += 2;
//			NFA::add(n.head, n1.head);
//			NFA::add(n.head, n2.head);
//			NFA::add(n1.tail, n.tail);
//			NFA::add(n2.tail, n.tail);
//			NFAS.push(n);
//		}
//		else {
//			if (1 == 1) {
//				NFA n = NFA();
//				tot += 2;
//				NFA n1 = NFAS.top();
//				NFAS.pop();
//				NFA::add(n1.tail, n1.head);
//				NFA::add(n.head, n1.head);
//				NFA::add(n1.tail, n.tail);
//				NFA::add(n.head, n.tail);
//				NFAS.push(n);
//			}
//			else {
//				NFA n1 = NFA();
//				tot += 2;
//				NFA n2 = NFAS.top();
//				NFAS.pop();
//				NFA::add(n2.tail, n1.head);
//				NFA::add(n2.tail, n1.tail);
//				NFA::add(n1.head, n2.head);
//				NFA::add(n1.head, n1.tail);
//				NFAS.push(n1);
//			}
//
//		}
//	}
//
//	return NFAS.top();
//}
//
////��ӡNFA
//void print_NFA(NFA n) {
//	printf("NFA:\n");
//	printf("��ʼ״̬Ϊ��%d  ��ֹ״̬Ϊ��%d\n", n.head->index, n.tail->index);
//	for (auto state : states) {
//		if (state.c != '#') {
//			printf("%d ->(%c) %d\n", state.index, state.c, state.to);
//		}
//		for (auto ep : state.eps) {
//			printf("%d ->(��) %d\n", state.index, ep);
//		}
//	}
//}
//
//NFA re2nfa(string& s) {
//
//	if (DEBUG) {
//		printf("DEBUG:ori:%s\n", s.c_str());
//	}
//	preprocess(s);
//	if (DEBUG) {
//		printf("DEBUG:pre:%s\n", s.c_str());
//	}
//
//	string s_post = in2post(s);
//	if (DEBUG) {
//		printf("DEBUG:post:%s\n", s_post.c_str());
//	}
//
//	NFA n = post2NFA(s_post);
//	if (DEBUG) {
//		printf("DEBUG:nfa is ok\n");
//		print_NFA(n);
//	}
//	return n;
//}
