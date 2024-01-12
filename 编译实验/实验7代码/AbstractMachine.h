#define _CRT_SECURE_NO_WARNINGS
#include <stack>
#include <string>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <map>
using namespace std;
#define MAX_SIZE 1000

// LOAD D 将 D 中的内容加载到操作数栈;
// LOADI 常量 将常量压入操作数栈;
// STO D 将操作数栈栈顶单元内容存入 D,且栈顶单元内容保持不变;
// POP 将操作数栈栈顶出栈;
// ADD 将次栈顶单元与栈顶单元内容出栈并相加,和置于栈顶;
// SUB 将次栈顶单元与栈顶单元内容出栈并相减,差置于栈顶;
// MULT 将次栈顶单元与栈顶单元内容出栈并相乘,积置于栈顶;
// DIV 将次栈顶单元与栈顶单元内容出栈并相除,商置于栈顶;
// BR lab 无条件转移到 lab
// BRF lab 检查栈顶单元逻辑值并出栈,若为假(0)则转移到 lab;
// EQ 将栈顶两单元做等于比较并出栈,并将结果真或假(1 或 0)置于栈顶;
// NOTEQ 将栈顶两单元做不等于比较并出栈,并将结果真或假(1 或 0)置于栈顶;
// GT 次栈顶大于栈顶操作数并出栈,则栈顶置 1,否则置 0;
// LES 次栈顶小于栈顶操作数并出栈,则栈顶置 1,否则置 0;
// GE 次栈顶大于等于栈顶操作数并出栈,则栈顶置 1,否则置 0
// LE 次栈顶小于等于栈顶操作数并出栈,则栈顶置 1,否则置 0;
// AND 将栈顶两单元做逻辑与运算并出栈,并将结果真或假(1 或 0)置于栈顶;
// OR 将栈顶两单元做逻辑或运算并出栈,并将结果真或假(1 或 0)置于栈顶;
// NOT 将栈顶的逻辑值取反;
// IN 从标准输入设备(键盘)读入一个整型数据,并入操作数栈;
// OUT 将栈顶单元内容出栈,并输出到标准输出设备上(显示器);
// STOP 停止执行;

bool match_(string& s,const char* t) {
	//检查s前缀是否与t匹配
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
			// 移除行末的换行符
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
				//将n压入栈中
				int n = stoi(c.substr(6));
				//cout << "n:" << n << endl;
				machine.push(n);
				code_cur++;
			}
			else if (match_(c, "LOAD")) {
				//LOAD n
				//将data[n]压入栈中
				int n = stoi(c.substr(5));
				machine.push(data[n]);
				code_cur++;
			}
			else if (match_(c, "STO")) {
				//STORE n
				//将栈顶元素存入data[n]
				int n = stoi(c.substr(4));
				data[n] = machine.top();
				code_cur++;
			}
			else if (match_(c, "POP")) {
				//POP
				//将栈顶元素弹出
				machine.pop();
				code_cur++;
			}
			else if (match_(c, "ADD")) {
				//ADD
				//将栈顶两个元素弹出，相加后将结果压入栈中
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				machine.push(a + b);
				code_cur++;
			}
			else if (match_(c, "SUB")) {
				//SUB
				//将栈顶两个元素弹出，相减后将结果压入栈中
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				machine.push(b - a);
				code_cur++;
			}
			else if (match_(c, "MULT")) {
				//MULT
				//将栈顶两个元素弹出，相乘后将结果压入栈中
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				machine.push(a * b);
				code_cur++;
			}
			else if (match_(c, "DIV")) {
				//DIV
				//将栈顶两个元素弹出，相除后将结果压入栈中
				int a = machine.top();
				machine.pop();
				int b = machine.top();
				machine.pop();
				machine.push(b / a);
				code_cur++;
			}
			else if (match_(c, "BRF")) {
				//检查栈顶单元逻辑值并出栈,若为假(0)则转移到 lab;
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
				//将栈顶两单元做等于比较并出栈,并将结果真或假(1 或 0)置于栈顶;
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
				//将栈顶两单元做不等于比较并出栈,并将结果真或假(1 或 0)置于栈顶;
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
				//次栈顶大于栈顶操作数并出栈,则栈顶置 1,否则置 0;
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
				//次栈顶小于栈顶操作数并出栈,则栈顶置 1,否则置 0;
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
				//次栈顶大于等于栈顶操作数并出栈,则栈顶置 1,否则置 0
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
				//次栈顶小于等于栈顶操作数并出栈,则栈顶置 1,否则置 0;
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
				//将栈顶两单元做逻辑与运算并出栈,并将结果真或假(1 或 0)置于栈顶;
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
				//将栈顶两单元做逻辑或运算并出栈,并将结果真或假(1 或 0)置于栈顶;
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
				//将栈顶的逻辑值取反;
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
				//从标准输入设备(键盘)读入一个整型数据,并入操作数栈;
				int a;
				cin >> a;
				machine.push(a);
				code_cur++;
			}
			else if (match_(c, "OUT")) {
				//将栈顶单元内容出栈,并输出到标准输出设备上(显示器);
				int a = machine.top();
				machine.pop();
				cout << a << endl;
				code_cur++;
			}
			else if (match_(c, "STOP")) {
				//停止执行;
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
			cout << "栈为空" << endl;
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