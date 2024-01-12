#include "re2nfa.h"
#include <fstream>


string words[] = { "if","else","for","while","do","int" };
class Rule {
public:
	string name;
	string re;
	DFA d;


	//���캯��
	Rule() {};
	Rule(string name, string re) :name(name), re(re) {};
};

class Machine {
#define R(x) Rules[x]
public:
	Rule Rules[10];
	ifstream inputFile;  // �����ļ���
	ofstream outputFile; // ����ļ���
	int lineCount; // ����
	map<char, char> change;//ת���ַ�
	//��������
	~Machine() {
		inputFile.close();
		outputFile.close();
	}

	//��ʼ��
	void init(string in,string out) {
		//��*�滻Ϊ��  ��Ҫ���ַ�ת��Ϊ�Ϸ��ַ����ҺϷ��ַ���Ҫ�޸ġ�
		//��Ҫ�Ļ��������Ҫת��Ԫ�ַ�
		string DIGIT = "(1|2|3|4|5|6|7|8|9|0)";
		string LETTER = "(a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|w|x|y|z|A|B|C|D|E|F|G|H|I|J|K|L|M|N|O|P|Q|R|S|T|U|V|W|X|Y|Z)";

		R(0) = Rule("letter", LETTER);
		R(1) = Rule("digit", DIGIT);
		R(2) = Rule("number", DIGIT + "*" + DIGIT);
		
		//DIGIT   -> #  
		//LETTER  -> @
		//*       -> ��
		//()      -> ����
		R(3) = Rule("identifier", LETTER + "(" + LETTER + "|" + DIGIT + ")*");
		R(3).re = "@|#";
		R(4) = Rule("singleword", "+|-|��|��|��|{|}|:|,|;");
		R(5) = Rule("division", "/");
		R(6) = Rule("comparision_single", "<|>|!|=");
		R(7) = Rule("comparision_double", ">=|<=|!=|==");
		R(8) = Rule("comment_first", "/��");
		R(9) = Rule("comment_last", "��/");

		for (int i = 0; i < 10; i++) {
			R(i).d = re2DFA(R(i).re);
			//cout << R(i).d;
		}
		//R(4).d = re2DFA(R(4).re);
		cout << "ok" << endl;

		//��ʼ��������Ա
		this->lineCount = 1;
		this->inputFile.open(in);
		this->outputFile.open(out);
		this->change['*'] = '��';
		this->change['('] = '��';
		this->change[')'] = '��';
	}

	// ���ļ��ж�ȡ�ַ�����¼����
	char read() {
		char c = EOF;

		inputFile.get(c);
		// ����ַ���ȡ�ļ�
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
		// ����ַ���ȡ�ļ�
		if (this->change.find(c) != change.end()) {
			return this->change[c];
		}
		else {
			return c;
		}
	}

	//�жϵ�ǰ״̬�Ƿ����ĳ��R���н�״̬
	bool if_set_intersection(set<int>& s1, int cur) {
		set<int>& s2 = R(cur - 1).d.end;
		std::set<int> intersection;
		std::set_intersection(s1.begin(), s1.end(), s2.begin(), s2.end(), std::inserter(intersection, intersection.begin()));
		return !intersection.empty();
	}
	//�жϵ�ǰ�Ƿ����ȫΪ-1��
	bool check(set<int>& s) {
		for (auto x : s) {
			if (x != -1) return true;
		}
		return false;
	}

	
	//�����Զ���
	void run() {
		char c = this->read();
		char p = EOF;
		set<int> last,first;
		for (int i = 0; i < 10; i++) {
			last.insert(Rules[i].d.start);
		}
		//��¼һ����ʼ״̬
		first = last;
		//��¼��ʷ�ַ�
		string his = "";
		while (c != EOF) {
			printf("DEBUG:%c\n", c);
			//���з�
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
				//��һ��
				for (auto l : last) {
					if (DFA::Tran.find({ l,c }) != DFA::Tran.end()) {
						cur.insert(DFA::Tran[{ l,c }]);
					}
				}
				last = cur;
				//�жϵ�ǰ�Ƿ�Ϊ�գ�Ϊ�վ����д�
				if (check(cur) == false || cur.size()==0) {
					printf("������%d��", this->lineCount);
					this->outputFile.close();
					return;
				}
				//�ж��Ƿ������Ӧ����ֹ�ַ�
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
								//��ȫ�����ΪID�����������ֹؼ���
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



