#define _CRT_SECURE_NO_WARNINGS

// Note:
// You are free to utilize any C++ standard library functions.
// Please ensure to include the necessary headers below.
// Avoid using <bits/stdc++.h> to prevent potential compilation errors that
//could result in a score of zero.
#include <iostream>
#include <complex>
#include <vector>
#include <utility>
#include <cmath>
#include <algorithm>
#include <set>
#include <chrono>
using namespace std;
typedef long double LD;
typedef complex<LD> Point;

#define DEBUG 0

//set the domain of x,y
#define max_x 10000
#define max_y 10000

const int TEST[] = { 100,1000,10000,100000 };
//You can add more functions here.
LD dis_squre(const Point& a, const Point& b) {
	return (a.imag() - b.imag()) * a.imag() - b.imag() + (a.real() - b.real()) * (a.real() - b.real());
}

//A sort helper
bool compareByReal(const Point& a, const Point& b) {
	return a.real() < b.real();
}

//A sort helper
bool compareByImag(const Point& a, const Point& b) {
	return a.imag() < b.imag();
}


//A func to convert vec to set
set<Point> v2s(vector<Point> P) {
	set<Point>s;
	for (auto& x : P) {
		s.insert(x);
	}
	return s;
}

//A func to help debug
void Print_v(const vector<Point> P) {
	printf("v:\n");
	for (auto &x : P) cout << x << endl;
}


//A helper func
pair<Point, Point> helper(vector<Point>& X , vector<Point>& Y) {
	//printf("ok1");
	//return make_pair(0, 0);
	int len = X.size();
	if (len <= 3) {
		if (len == 1) {
			return make_pair(Point{ 0,0 }, Point{ 0,0 });
		}
		else if (len == 2) {
			return make_pair(X[0], X[1]);
		}
		else {
			LD a = dis_squre(X[0], X[1]);
			LD b = dis_squre(X[0], X[2]);
			LD c = dis_squre(X[2], X[1]);
			if (a < b and a < c) return make_pair(X[0], X[1]);
			else if (b < a and b < c) return make_pair(X[0], X[2]);
			else return make_pair(X[2], X[1]);
		}
	}
	else {
		int half = X.size()/2;
		vector<Point> XL(half);
		vector<Point> XR(X.size() - half);
		for (int i = 0; i < half; i++) {
			XL[i] = X[i];
		}
		for (int i= half; i < X.size(); i++) {
			XR[i - half] = X[i];
		}
		if (DEBUG) {
			printf("OK1\n");
			Print_v(X);
			Print_v(XL);
			Print_v(XR);
		}
		vector<Point> YL, YR;
		auto P = v2s(XL);
		for (int i = 0; i < Y.size(); i++) {
			if (P.find(Y[i]) != P.end()) {
				YL.push_back(Y[i]);
			}
			else YR.push_back(Y[i]);
		}
		if (DEBUG) {
			printf("\n\n\n");
			printf("SET:\n");
			for (auto x : P) {
				cout << x << endl;
			}
			Print_v(Y);
			Print_v(YL);
			Print_v(YR);
		}
		pair<Point, Point> ans;
		pair<Point, Point> pair1 = helper(XL, YL);
		LD dis1 = dis_squre(pair1.first,pair1.second);
		pair<Point, Point> pair2 = helper(XR, YR);
		LD dis2 = dis_squre(pair2.first,pair2.second);
		LD d = 0;
		if (dis1 < dis2) {
			d = dis1;
			ans = pair1;
		}
		else {
			d = dis2;
			ans = pair2;
		}
		LD mid = X[half - 1].real();
		if (DEBUG) {
			cout << "DEBUG:mid:" << mid << endl;
		}
		vector<Point> Y_;
		for (auto &point : Y) {
			if (mid - d <= point.real() and point.real() <= d + mid) {
				Y_.push_back(point);
			}
		}
		for (int i = 0; i < Y_.size(); i++) {
			for (int j = 1; j <= 7; j++) {
				if (i + j < Y_.size()) {
					LD d_ = dis_squre(Y_[i], Y_[i + j]);
					if (d_ < d) {
						d = d_;
						ans = make_pair(Y_[i] ,Y_[i,j]);
					}
				}	
			}
		}
		return ans;
	}
}


bool operator<(const Point& lhs, const Point& rhs) {
	// 在这里定义如何比较两个Point对象的大小
	if (lhs.real() == rhs.real()) {
		return lhs.imag() < rhs.imag();
	}
	return lhs.real() < rhs.real();
}



pair<Point, Point> closest_pair(vector<Point>& P)
{
	// Insert you code here. You can add more functions but do not change the
	//definition of this function.
	sort(P.begin(), P.end(), compareByReal);
	vector<Point> P_y(P);
	sort(P_y.begin(), P_y.end(), compareByImag);

	return helper(P, P_y);
}

//A fuction used to make point data.
vector<Point> make_data(int num) {
	vector<Point> ans;
	ans.resize(num);
	for (int i = 0; i < num; i++) {
		LD x = ((LD)rand() / RAND_MAX) * max_x;
		LD y = ((LD)rand() / RAND_MAX) * max_y;
		ans[i] = (Point{ x,y });
	}
	return ans;
}

//A function used to printf point pair.
void print_pair(const pair<Point, Point>& p) {
	cout << p.first << endl << p.second << endl;
}

int main(int argc, const char* argv[]) {
	// You can insert code here to test you function.
	srand(time(nullptr));

	for (int i = 0; i < sizeof(TEST)/sizeof(TEST[0]);i++) {
		auto start_time = chrono::high_resolution_clock::now();
		int num = TEST[i];
		vector<Point> test = make_data(num);
		//Print_v(test);
		auto ans = closest_pair(test);
		cout << "最近点对\n";
		print_pair(ans);
		auto end_time = chrono::high_resolution_clock::now();

		// 计算时间差
		auto duration = chrono::duration_cast<chrono::microseconds>(end_time - start_time);
		printf("测试点%d:运行时间为:", i + 1);
		cout << duration.count() << " ms" << endl;
		
	}

	return 0;
}
// Please refrain from including any code beyond the main function,
// as any additional code will be removed during the code evaluation process.