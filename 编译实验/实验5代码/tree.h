#define _CRT_SECURE_NO_WARNINGS
#include<string>
#include<vector>
#include<iostream>
using namespace std;

//一课可以自上而下构造的树
class Tree {
public:
	string label;
	vector<Tree*> branches;
	Tree* parent;
	Tree* brother;

	Tree(string label, vector<Tree*> branches = vector<Tree*>(), Tree* parent = nullptr, Tree* bro =nullptr) {
		this->label = label;
		this->branches = branches;
		this->parent = parent;
		this->brother = bro;
		if (branches.size() >= 1) {
			int i = 0;
			for (; i < branches.size() - 1; i++) {
				branches[i]->parent = this;
				branches[i]->brother = branches[i + 1];
			}
			branches[i]->parent = this;
		}
		
	}

	bool is_leaf(){
		return branches.size() == 0;
	}

	Tree* next() {
		if (brother != nullptr) return brother;
		else {
			if (parent != nullptr) return parent->next();
			else return nullptr;
		}
	}

	void add_child(Tree* child) {

		this->branches.push_back(child);
		child->parent = this;
		int len = branches.size();
		if (len >= 2) {
			branches[len - 2]->brother = child;
		}
	}


	void Print(int indent) {
		for (int i = 0; i < indent; i++) {
			cout << "\t";
		}
		cout << label << endl;
		for (auto b : branches) {
			b->Print(indent + 1);
		}
	}

};