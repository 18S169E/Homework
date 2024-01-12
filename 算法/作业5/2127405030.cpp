// Note:
// You are free to utilize any C++ standard library functions.
// Please ensure to include the necessary headers below.
// Avoid using <bits/stdc++.h> to prevent potential compilation errors that 
//could result in a score of zero.
const int N = 10;
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
ostream& operator<<(ostream& os, const vector<int>& v) {
	for (auto e : v)
		os << e << ' ';
	return os;
}
ostream& operator<<(ostream& os, const vector<vector<int>>& v) {
	for (auto e : v)
		os << e << '\n';
	return os;
}
vector<int> find_lis_dp1(const vector<int>& s, int n);
vector<int> find_lis_dp2(const vector<int>& s, int n);
vector<int> find_lis_dp3(const vector<int>& s, int n);

// insert code here...
// algorithm 1 to compute the length of LIS
int lis_dp1(const vector<int>& s, int n) {
	return find_lis_dp1(s, n).size();
}


//algorithm 2 to compute the length of LIS
int lis_dp2(const vector<int>& s, int n) {
	return find_lis_dp2(s, n).size();
}

// algorithm 3 to compute the length of lis
int lis_dp3(const vector<int>& s, int n) {
	return find_lis_dp3(s, n).size();
}
//// revised algorithm 1 to compute a LIS
vector<int> find_lis_dp1(const vector<int>& s, int n) {
	vector<vector<int>> L(N,vector<int>(N));
	L[0] = vector<int>{ s[0] };
	//for (int i = 0; i <= n; i++) {
	//	L[i] = vector<int>{ s[i] };
	//}
	for (int k = 1; k <= n; k++) {
		L[k] = vector<int>{ s[k] };
		int max_ = 1;
		int index = k;
		for (int i = 0; i <= k - 1; i++) {
			if (s[k] > s[i]) {
				int cur = L[i].size() + 1;
				if (cur > max_) {
					max_ = cur;
					index = i;
				}
			
			}

		}
		if (max_ > 1) {
			L[k] = vector<int>(L[index].begin(), L[index].end());
			if (s[k] > s[index]) L[k].push_back(s[k]);
		}
	}


	int max_ = 1;
	int index = 0;
	for (int i = 0; i <= n; i++) {
		if (L[i].size() > max_) {
			max_ = L[i].size();
			index = i;
		}
		//cout << i << " " << L[i] << endl;
	}
	return L[index];
}


// revised algorithm 2 to compute a LIS
vector<int> find_lis_dp2(const vector<int>& s, int n) {
	int BLOCK = -1e9;
	vector<int>ss;
	ss.push_back(BLOCK);
	for (auto x : s) {
		ss.push_back(x);
	}
	vector<vector<vector<int>>> L(N+1, vector<vector<int>>(N+1,vector<int>()));
	
	for (int j = n + 2; j >= 1; j--) {
		for (int i = 0; i <= n + 1; i++) {
			if (j == n + 2) {
				continue;
			}
			else {
				if (ss[i] >= ss[j]) {
					L[i][j] = L[i][j + 1];
				}
				else {
					int len1 = L[i][j + 1].size();
					int len2 = L[j][j + 1].size() + 1;
					if (len1 < len2) {
						vector<int> temp;
						temp.push_back(ss[j]);
						for (auto t : L[j][j + 1]) {
							temp.push_back(t);
						}
						L[i][j] = temp;
					}
					else {
						L[i][j] = L[i][j + 1];
					}
				}
			}
		}
	}
	
	return L[0][1];
}
	
// revised algorithm 3 to compute a LIS
vector<int> find_lis_dp3(const vector<int>& s, int n) {
	int len = 1;
	n++;
	int BLOCK = -1e9;
	vector<int> d(n + 1, BLOCK);
	d[len] = s[0];
	for (int i = 1; i < n; ++i) {
		if (s[i] > d[len]) {
			d[++len] = s[i];
		}
		else {
			int l = 1, r = len, pos = 0; 
			while (l <= r) {
				int mid = (l + r) >> 1;
				if (d[mid] < s[i]) {
					pos = mid;
					l = mid + 1;
				}
				else {
					r = mid - 1;
				}
			}
			d[pos + 1] = s[i];
		}
	}
	vector<int> ans;
	for (auto t : d) {
		if (t != BLOCK) ans.push_back(t);
	}
	return ans;
}

int main(int argc, const char* argv[]) {
	//ans = -1 1 3 5(4) 7   len = 5

	vector<int> s{ -1, 8, 3, 6, 1, 3, 5, 4, 7 };
	cout << lis_dp1(s, s.size() - 1) << endl;
	cout << lis_dp2(s, s.size() - 1) << endl;
	cout << lis_dp3(s, s.size() - 1) << endl;
	cout << find_lis_dp1(s, s.size() - 1) << endl;
	cout << find_lis_dp2(s, s.size() - 1) << endl;
	cout << find_lis_dp3(s, s.size() - 1) << endl;
	return 0;
}
// Please refrain from including any code beyond the main function,
// as any additional code will be removed during the code evaluation 
//process.