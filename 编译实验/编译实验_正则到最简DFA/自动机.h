#include "re2nfa.h"
#include <fstream>


string words[] = { "if","else","for","while","do","int" };
class Rule {
public:
	string name;
	string re;
	DFA d;


	//构造函数
	Rule() {};
	Rule(string name, string re) :name(name), re(re) {};
};

class Machine {
#define R(x) Rules[x]
public:
	Rule Rules[10];
	ifstream inputFile;  // 输入文件流
	ofstream outputFile; // 输出文件流
	int lineCount; // 行数
	map<char, char> change;//转换字符
	//析构函数
	~Machine() {
		inputFile.close();
		outputFile.close();
	}

	//初始化
	void init(string in,string out) {
		//将*替换为×  需要将字符转换为合法字符，且合法字符集要修改。
		//必要的话最后的输出要转回元字符
		string DIGIT = "(1|2|3|4|5|6|7|8|9|0)";
		string LETTER = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";

		R(0) = Rule("letter", LETTER);
		R(1) = Rule("digit", DIGIT);
		R(2) = Rule("number", DIGIT + "*" + DIGIT);
		
		//DIGIT   -> #  
		//LETTER  -> @
		//*       -> ×
		//()      -> （）
		R(3) = Rule("identifier", LETTER + "(" + LETTER + "|" + DIGIT + ")*");
		R(3).re = "@|#";
		R(4) = Rule("singleword", "+|-|×|（|）|{|}|:|,|;");
		R(5) = Rule("division", "/");
		R(6) = Rule("comparision_single", "<|>|!|=");
		R(7) = Rule("comparision_double", ">=|<=|!=|==");
		R(8) = Rule("comment_first", "/×");
		R(9) = Rule("comment_last", "×/");

		for (int i = 0; i < 10; i++) {
			R(i).d = re2DFA(R(i).re);
			//cout << R(i).d;
		}
		//R(4).d = re2DFA(R(4).re);
		cout << "ok" << endl;

		//初始化其他成员
		this->lineCount = 1;
		this->inputFile.open(in);
		this->outputFile.open(out);
		this->change['*'] = '×';
		this->change['('] = '（';
		this->change[')'] = '）';
	}

	// 从文件中读取字符并记录行数
	char read() {
		char c = EOF;

		inputFile.get(c);
		// 逐个字符读取文件
		if (this->change.find(c) != change.end()) {
			return this->change[c];
		}
		else {
			return c;
		}

	}

	char peak() {
		char c = EOF;

		c = inputFile.peek();
		// 逐个字符读取文件
		if (this->change.find(c) != change.end()) {
			return this->change[c];
		}
		else {
			return c;
		}
	}

	//判断当前状态是否包含某个R的中介状态
	bool if_set_intersection(set<int>& s1, int cur) {
		set<int>& s2 = R(cur - 1).d.end;
		std::set<int> intersection;
		std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(intersection, intersection.begin()));
		return !intersection.empty();
	}
	//判断当前是否错误（全为-1）
	bool check(set<int>& s) {
		for (auto x : s) {
			if (x != -1) return true;
		}
		return false;
	}

	
	//运行自动机
	void run() {
		char c = this->read();
		char p = EOF;
		set<int> last,first;
		for (int i = 0; i < 10; i++) {
			last.insert(Rules[i].d.start);
		}
		//记录一个初始状态
		first = last;
		//记录历史字符
		string his = "";
		while (c != EOF) {
			printf("DEBUG:%c\n", c);
			//换行符
			if (c == '\n') {
				c = this->read();
				this->lineCount++;
			}
			if (isspace(c)) {
				c = this->read();
			}
			else {
				his += c;
				set<int> cur;
				//走一步
				for (auto l : last) {
					if (DFA::Tran.find({ l,c }) != DFA::Tran.end()) {
						cur.insert(DFA::Tran[{ l,c }]);
					}
				}
				last = cur;
				//判断当前是否为空，为空就是有错
				if (check(cur) == false || cur.size()==0) {
					printf("错误，于%d行", this->lineCount);
					this->outputFile.close();
					return;
				}
				//判断是否包含对应的中止字符
				else {
					//identifier 4
					if (if_set_intersection(cur, 4)) {
						c = this->read();
						//his += c;
						if (c == EOF||!isalnum(c)) {
							bool if_print = false;
							for (auto word : words) {
								if (word == his) {
									this->outputFile << his<<"\t" << his << endl;
									his = "";
									last = first;
									if_print = true;
									break;
								}
							}
							if (if_print) {}
							else {
								//先全部输出为ID，后续再区分关键字
								this->outputFile << "ID\t" << his << endl;
								his = "";
								last = first;
							}
						}
					}
					else if (if_set_intersection(cur, 5)) {
						c = this->read();
						this->outputFile << his << "\t" << his << endl;
						his = "";
						last = first;
					}
				}
			}
		}

	}
};



