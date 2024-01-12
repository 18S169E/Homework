#define _CRT_SECURE_NO_WARNINGS
#include <stack>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;
#define MAX_SIZE 1000

// LOAD D �� D �е����ݼ��ص�������ջ;
// LOADI ���� ������ѹ�������ջ;
// STO D ��������ջջ����Ԫ���ݴ��� D,��ջ����Ԫ���ݱ��ֲ���;
// POP ��������ջջ����ջ;
// ADD ����ջ����Ԫ��ջ����Ԫ���ݳ�ջ�����,������ջ��;
// SUB ����ջ����Ԫ��ջ����Ԫ���ݳ�ջ�����,������ջ��;
// MULT ����ջ����Ԫ��ջ����Ԫ���ݳ�ջ�����,������ջ��;
// DIV ����ջ����Ԫ��ջ����Ԫ���ݳ�ջ�����,������ջ��;
// BR lab ������ת�Ƶ� lab
// BRF lab ���ջ����Ԫ�߼�ֵ����ջ,��Ϊ��(0)��ת�Ƶ� lab;
// EQ ��ջ������Ԫ�����ڱȽϲ���ջ,�����������(1 �� 0)����ջ��;
// NOTEQ ��ջ������Ԫ�������ڱȽϲ���ջ,�����������(1 �� 0)����ջ��;
// GT ��ջ������ջ������������ջ,��ջ���� 1,������ 0;
// LES ��ջ��С��ջ������������ջ,��ջ���� 1,������ 0;
// GE ��ջ�����ڵ���ջ������������ջ,��ջ���� 1,������ 0
// LE ��ջ��С�ڵ���ջ������������ջ,��ջ���� 1,������ 0;
// AND ��ջ������Ԫ���߼������㲢��ջ,�����������(1 �� 0)����ջ��;
// OR ��ջ������Ԫ���߼������㲢��ջ,�����������(1 �� 0)����ջ��;
// NOT ��ջ�����߼�ֵȡ��;
// IN �ӱ�׼�����豸(����)����һ����������,���������ջ;
// OUT ��ջ����Ԫ���ݳ�ջ,���������׼����豸��(��ʾ��);
// STOP ִֹͣ��;

bool match_(string& s,const char* t) {
	//���sǰ׺�Ƿ���tƥ��
	for (int i = 0; t[i] != '\0'; i++) {
		if (s[i] != t[i]) {
			return false;
		}
	}
	return true;
}

class AbstractMachine {
public:
	//stack
	stack<int> machine;
	int* data;
	string* code;
	map<string, int> label2line;
	int code_cur;
	int code_count;
	//init
	AbstractMachine() : data(new int[MAX_SIZE]), code(new std::string[MAX_SIZE]), code_cur(0), code_count(0) {
		for (int i = 0; i < MAX_SIZE; i++) {
			data[i] = 0;
			code[i] = "";
		}
	}

	//get code from file
	void getCode(string filename) {
		ifstream file(filename);
		string line;
		while (getline(file, line)) {
			// �Ƴ���ĩ�Ļ��з�
			if (!line.empty() && line.back() == '\n') {
				line.pop_back();
			}
			code[code_count++] = line;
			//If the first 5 characters are "LABEL" and the last character is ":"
			//then map the corresponding label to the next line number
			if (line.substr(0, 5) == "LABEL" && line.back() == ':') {
				//except the last character
				label2line[line.substr(0, line.size() - 1)] = code_count;
			}
		}
		file.close();
	}

	//run machine
	void run() {
		while (code_cur < code_count) {
			string& c = code[code_cur];
			if (match_(c, "LOADI")) {
				//LOADI n
				//��nѹ��ջ��
				int n = stoi(c.substr(6));
				//cout << "n:" << n << endl;
				machine.push(n);
				code_cur++;
			}
			else if (match_(c, "LOAD")) {
				//LOAD n
				//��data[n]ѹ��ջ��
				int n = stoi(c.substr(5));
				machine.push(data[n]);
				code_cur++;
			}
			else if (match_(c, "STO")) {
				//STORE n
				//��ջ��Ԫ�ش���data[n]
				int n = stoi(c.substr(4));
				data[n] = machine.top();
				code_cur++;
			}
			else if (match_(c, "POP")) {
				//POP
				//��ջ��Ԫ�ص���
				machine.pop();
				code_cur++;
			}
			else if (match_(c, "ADD")) {
				//ADD
				//��ջ������Ԫ�ص�������Ӻ󽫽��ѹ��ջ��
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				machine.push(a + b);
				code_cur++;
			}
			else if (match_(c, "SUB")) {
				//SUB
				//��ջ������Ԫ�ص���������󽫽��ѹ��ջ��
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				machine.push(b - a);
				code_cur++;
			}
			else if (match_(c, "MULT")) {
				//MULT
				//��ջ������Ԫ�ص�������˺󽫽��ѹ��ջ��
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				machine.push(a * b);
				code_cur++;
			}
			else if (match_(c, "DIV")) {
				//DIV
				//��ջ������Ԫ�ص���������󽫽��ѹ��ջ��
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				machine.push(b / a);
				code_cur++;
			}
			else if (match_(c, "BRF")) {
				//���ջ����Ԫ�߼�ֵ����ջ,��Ϊ��(0)��ת�Ƶ� lab;
				int f = machine.top();
				machine.pop();
				if (f == 0) {
					string lab = c.substr(4);
					code_cur = label2line[lab];
				}
				else {
					code_cur++;
				}
			}
			else if (match_(c, "BR")) {
				string lab = c.substr(3);
				code_cur = label2line[lab];
			}
			else if (match_(c, "EQ")) {
				//��ջ������Ԫ�����ڱȽϲ���ջ,�����������(1 �� 0)����ջ��;
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				if (a == b) {
					machine.push(1);
				}
				else {
					machine.push(0);
				}
				code_cur++;
			}
			else if (match_(c, "NOTEQ")) {
				//��ջ������Ԫ�������ڱȽϲ���ջ,�����������(1 �� 0)����ջ��;
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				if (a != b) {
					machine.push(1);
				}
				else {
					machine.push(0);
				}
				code_cur++;
			}
			else if (match_(c, "GT")) {
				//��ջ������ջ������������ջ,��ջ���� 1,������ 0;
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				if (b > a) {
					machine.push(1);
				}
				else {
					machine.push(0);
				}
				code_cur++;
			}
			else if (match_(c, "LES")) {
				//��ջ��С��ջ������������ջ,��ջ���� 1,������ 0;
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				if (b < a) {
					machine.push(1);
				}
				else {
					machine.push(0);
				}
				code_cur++;
			}
			else if (match_(c, "GE")) {
				//��ջ�����ڵ���ջ������������ջ,��ջ���� 1,������ 0
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				if (b >= a) {
					machine.push(1);
				}
				else {
					machine.push(0);
				}
			}
			else if (match_(c, "LE")) {
				//��ջ��С�ڵ���ջ������������ջ,��ջ���� 1,������ 0;
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				if (b <= a) {
					machine.push(1);
				}
				else {
					machine.push(0);
				}
				code_cur++;
			}
			else if (match_(c, "AND")) {
				//��ջ������Ԫ���߼������㲢��ջ,�����������(1 �� 0)����ջ��;
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				if (a == 1 && b == 1) {
					machine.push(1);
				}
				else {
					machine.push(0);
				}
				code_cur++;
			}
			else if (match_(c, "OR")) {
				//��ջ������Ԫ���߼������㲢��ջ,�����������(1 �� 0)����ջ��;
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				if (a == 1 || b == 1) {
					machine.push(1);
				}
				else {
					machine.push(0);
				}
				code_cur++;
			}
			else if (match_(c, "NOT")) {
				//��ջ�����߼�ֵȡ��;
				int a = machine.top();
				machine.pop();
				if (a == 1) {
					machine.push(0);
				}
				else {
					machine.push(1);
				}
				code_cur++;
			}
			else if (match_(c, "IN")) {
				//�ӱ�׼�����豸(����)����һ����������,���������ջ;
				int a;
				cin >> a;
				machine.push(a);
				code_cur++;
			}
			else if (match_(c, "OUT")) {
				//��ջ����Ԫ���ݳ�ջ,���������׼����豸��(��ʾ��);
				int a = machine.top();
				machine.pop();
				cout << a << endl;
				code_cur++;
			}
			else if (match_(c, "STOP")) {
				//ִֹͣ��;
				break;
			}
			else {
				cout << "error" << endl;
				break;
			}
		}
	}


	void check_result() {
		if (machine.empty()) {
			cout << "ջΪ��" << endl;
		}
		else {
			cout << machine.top() << endl;
		}
	}

	void print_code() {
		for (int i = 0; i < MAX_SIZE; i++) {
			if (code[i] == "") {
				break;
			}
			cout << code[i] << endl;
		}
	}
};