#pragma once
#define _CRT_SECURE_NO_WARNINGS
//��������ʽ�ַ��� �����﷨�������������﷨��������
// 1. ��ȡ����ʽ ���ַ�������Ϊ��Ӧ�Ľṹ
// 2. �����������߱� ����Ҫ�õ�first��follow���죩
// 3. ��ȡ���봮�����з��� ��������  ��ȡ��ר�ŵĺ���

#include <map>
#include <vector>
#include <set>
#include <iostream>
#include <string>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <stack>
#include "tree.h"
using namespace std;
char token[20], token1[40]; extern char Scanout[300];
extern map<int, int> word2line;
FILE* fp;
int line_cnt = 0;

#define DEBUG 1
#define Symbol string
#define Symbols vector<Symbol>


void gettoken() {
    fscanf(fp, "%s %s\n", token, token1);
    line_cnt++;
}

void printtoken() {
    printf("%s\t%s\n", token, token1);
}

void gandp() {
    gettoken();
    printtoken();
}

//����ʽ ��left �� right ����  ÿһ������ַ�������ʱ���õ�������һ�����ʾ��
class Produce {
public:
    Symbol left;
    Symbols right;
    
    Produce() {};
    Produce(Symbol left, Symbols right) :left(left), right(right) {};


    friend std::ostream& operator<<(std::ostream& os, const Produce& produce) {
        os << produce.left << "\t->\t";
        for (auto r : produce.right) {
            cout << r << " ";
        }
        cout << endl;
        return os;
    }
};

class LLparse {
public:
    //����ʽ
    bool make_tree = false;
    vector<Produce> Produces;
    set<Symbol> Vt;
    set<Symbol> Vn;
    map<Symbol, set<Symbol>> FIRST;
    map<Symbol, set<Symbol>> FOLLOW;
    Symbol start = "";
    map<pair<Symbol, Symbol>, Produce> M;
    Tree* grammer_tree = nullptr;
    string input_file;
    int line = 0;
    vector<string> cycle;
    

    //������ʽ���ַ�����ʽ����Ϊ���ݽṹ
    void Produces_struct() {
        string line;
        ifstream file(input_file.c_str());
        while (getline(file, line)) {
            istringstream iss(line);
            Symbol left, right;
            iss >> left;
            Vn.insert(left);
            Symbols body;
            while (iss >> right) {
                if (right == "|") {
                    Produces.push_back(Produce(left, body));
                    body.clear();
                }
                else {
                    body.push_back(right);
                }
            }
            Produces.push_back(Produce(left, body));
        }

        for (auto p : Produces) {
            for (auto r : p.right) {
                if (Vn.find(r) == Vn.end()) {
                    Vt.insert(r);
                }
            }
        }
        //eps������ս��
        Vt.erase("eps");
        Vt.insert("$");
        start = Produces[0].left;

    }

    //��ȡ x/x... �� FIRST
    set<Symbol> get_first(Symbol x) {
        //printf("CUR:%s\n", x.c_str());
        if (FIRST.find(x) != FIRST.end()) {
            return FIRST[x];
        }
        else {
            set<string> ans;
            if (Vt.find(x) != Vt.end()) {
                ans.insert(x);
            }
            else {
                for (auto p : Produces) {
                    if (p.left == x) {
                        if (p.right[0] == "eps") {
                            ans.insert("eps");
                        }
                        else {
                            string a = p.right[0];
                            if (Vt.find(a) != Vt.end()) {
                                ans.insert(a);
                            }
                            //�ײ����ս��
                            else {
                                //��һ��һ���Ƿ��ս�� �ѳ�eps���� Ȼ����ݵ�ǰ���ж��Ƿ�����һ��
                                int index = 0;
                                bool eps_exist;
                                do {
                                    //���ܵ����Լ����������ѭ��
                                    if (p.right[index] == x) continue;
                                    set<string> temp = get_first(p.right[index]);
                                    eps_exist = false;
                                    for (auto t : temp) {
                                        if (t != "eps") {
                                            ans.insert(t);
                                        }
                                        else eps_exist = true;
                                    }
                                    index++;
                                } while (eps_exist && index < p.right.size() && Vn.find(p.right[index]) != Vn.end());
                                if (eps_exist) ans.insert("eps");
                            }
                        }

                    }
                }

            }
            FIRST[x] = ans;
            return ans;
        }
    }
    set<Symbol> get_first(Symbols xs) {
        set<Symbol> ans;
        bool all_eps = true;
        for (Symbol x : xs) {
            set<Symbol> f = get_first(x);
            if (f.find("eps") != f.end()) {
                for (Symbol _ : f) {
                    if (_ != "eps") ans.insert(_);
                }
            }
            else {
                for (Symbol _ : f) {
                    ans.insert(_);
                }
                all_eps = false;
                break;
            }
        }
        if (all_eps) {
            ans.insert("eps");
        }
        return ans;
    }
    set<Symbol> get_follow(Symbol x) {
        
        //cout << "CUR " << x << endl;
        //������ֱ�ӷ���{}
        for (auto c : cycle) {
            if (c == x) return {};
        }
        if (FOLLOW.find(x) != FOLLOW.end()) {
            return FOLLOW[x];
        }
        else {
            set<string> ans;
            if (x == start) {
                ans.insert("$");
            }
            for (auto p : Produces) {
                for (int i = 0; i < p.right.size(); i++) {
                    if (p.right[i] == x) {
                        //���һ��Ԫ��
                        if (i == p.right.size() - 1) {
                            if (p.left != x) {
                                cycle.push_back(x);
                                set<string> temp = get_follow(p.left);
                                cycle.clear();
                                for (auto t : temp) {
                                    ans.insert(t);
                                }
                            }
                        }
                        else {
                            //�����ұ����� Symbols 
                           // string a = p.right[i + 1];
                            Symbols s;
                            for (int j = i + 1; j < p.right.size(); j++) {
                                s.push_back(p.right[j]);
                            }
                            bool eps_exist = false;
                            //��ֹ A - > b A�����
                            
                            for (auto right : get_first(s)) {
                                if (right != "eps") {
                                    ans.insert(right);
                                }
                                else eps_exist = true;
                            }
                            if (eps_exist) {
                                cycle.push_back(x);
                                set<string> temp = get_follow(p.left);
                                cycle.clear();
                                for (auto t : temp) {
                                    ans.insert(t);
                                }
                            }
                        }
                    }
                }
            }
            FOLLOW[x] = ans;
            return ans;
        }
    }

    void make_table() {
        set<Symbol> _({ "eps" });
        FIRST["eps"] = _ ;
        get_first("declaration_list");
        for (auto vn : Vn) {
            FIRST[vn] = get_first(vn);
        }

        for (auto vt : Vt) {
            FIRST[vt] = get_first(vt);
        }

        /*{
            cout << "FIRST��" << endl;
            for (auto vn : Vn) {
                cout << vn << " ";
                for (auto x : FIRST[vn]) {
                    cout << x << " ";
                }
                cout << endl;
            }
            for (auto vt : Vt) {
                cout << vt << " ";
                for (auto x : FIRST[vt]) {
                    cout << x << " ";
                }
                cout << endl;
            }

        }*/

        for (auto vn : Vn) {
            FOLLOW[vn] = get_follow(vn);
        }

        //��map ���� M
        for (auto produce : Produces) {
            Symbols& alpha = produce.right;
            Symbol& A = produce.left;
            set<Symbol> FIRST_alpha = get_first(alpha);
            for (auto a : FIRST_alpha) {
                if (Vt.find(a) != Vt.end()) {
                    M[{A, a}] = produce;
                }
            }

            if (FIRST_alpha.find("eps") != FIRST_alpha.end()) {
                set<Symbol> FOLLOW_A = get_follow(A);
                for (auto b : FOLLOW_A) {
                    if (Vt.find(b) != Vt.end()) {
                        M[{A, b}] = produce;
                    }
                }

            }
        }
    }

    //�Ƿ�Ҫ�����й����й�����  vĩβΪ��$��
    int run(vector<string> &v) {
        //init
        stack<Symbol> s;
        s.push("$");
        s.push(this->start);
        int ip = 0;
        Symbol X = s.top();
        string a = v[ip];
        Tree* cur = nullptr;
        if (this->make_tree) {
            this->grammer_tree = new Tree(this->start);
            cur = this->grammer_tree;
        }
        while (ip<v.size()) {
            
            if (Vt.find(X) != Vt.end()) {
                if (X == a) {
                    s.pop();
                    ip++;
                    if (this->make_tree) {
                        if(cur != nullptr) cur = cur->next();
                    }
                }
                else return 1;
            }
            else if (M.find({ X,a }) != M.end()) {
                s.pop();
                Produce p = M[{ X, a }];
                for (int i = p.right.size() - 1; i>=0; i--) {
                    if(p.right[i] != "eps")
                        s.push(p.right[i]);
                }
                if (this->make_tree) {

                    for (auto _ : p.right) {
                        Tree* t = new Tree(_);
                        cur->add_child(t);
                    }
                    cur = cur->branches[0];
                    if (cur->label == "eps") cur = cur->next();
                }
                //cout << p;
            }
            else return 2;
            
            if (ip >= v.size()) {
                if (s.empty()) return 0;
                else return 3;
            }
            X = s.top();
            a = v[ip];
            
        }
        return 0;
    }


    int parse(vector<string> v) {
        Produces_struct();
        if (DEBUG) {
            cout << "���ս��:";
            for (auto vn : Vn) cout << vn << " ";
            cout << endl;
            cout << "�ս����";
            for (auto vt : Vt) cout << vt << " ";
            cout << endl;;
            cout << "����ʽ:" << endl;
            for (auto p : Produces) {
                cout << "Left:" << p.left;
                cout << "\tRight:";
                for (auto right : p.right) {
                    cout << right << " ";
                }
                cout << endl;
            }
        }

        make_table();
        if (DEBUG) {
            cout << "FIRST��" << endl;
            for (auto vn : Vn) {
                cout << vn << " ";
                for (auto x : FIRST[vn]) {
                    cout << x << " ";
                }
                cout << endl;
            }
            for (auto vt : Vt) {
                    cout << vt << " ";
                    for (auto x : FIRST[vt]) {
                        cout << x << " ";
                    }
                    cout << endl;
            }
            cout << "FOLLOW��" << endl;
            for (auto vn : Vn) {
                cout << vn << " ";
                for (auto x : FOLLOW[vn]) {
                    cout << x << " ";
                }
                cout << endl;
            }

            cout << "TABLE" << endl;
            for (auto t : M) {
                printf("[%s,%s]\t", t.first.first.c_str(), t.first.second.c_str());
                cout << t.second << endl;
            }
        }


        int error = run(v);
        if (error) {
            printf("������� %d ��\n", word2line[line_cnt]);
            printf("Ϊ�� %d �����ʣ� %s", line_cnt, token);
            if (error == 1) {
                cout << "��ƥ��" << endl;
            }
            else if(error == 2){
                cout << "����M��" << endl;
            }
            else {
                cout << "����������ƥ��" << endl;
            }
            //cout << "����" << endl;
        }
        else {
            cout << "�﷨������ȷִ��" << endl;
        }
        return error;
    }
};

int LLPARSE() {
    LLparse parse;
    int es = 0;
    
    if ((fp = fopen(Scanout, "r")) == NULL)
    {
        printf("\n Can not open % S\n", Scanout);
        es = 10;
    }
    if (es == 0) {
        vector<string> v;
        while (!feof(fp)) {
            gettoken();
            if (token && token1) {
                v.push_back(token);
            }
        }
        v.push_back("$");
        //parse.make_tree = true;
        parse.input_file = "TEST_production.txt";
        es = parse.parse(v);
        return es;
    }

}