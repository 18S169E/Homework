// Note:
// You are free to utilize any C++ standard library functions.
// Please ensure to include the necessary headers below.
// Avoid using <bits/stdc++.h> to prevent potential compilation errors that could
//result in a score of zero.
#include <iostream>
#include <complex>
#include <vector>
#include <utility>
#include <list>
#include <algorithm>
using namespace std;
typedef long double LD;
typedef complex<LD> Point;
#define POINTSNUM 11
Point Points[POINTSNUM];
#define x real()
#define y imag()

#define LEFT 1
#define RIGHT -1
#define IN 0

//代码不考虑错误情况，也就是说必须能构成凸包

void print_list(string s, list<Point> l) {
	cout << s << endl;
	for (auto p : l)cout << p << endl;
}
// 重载输出运算符 <<，以便能够通过 cout 输出 Point 对象
std::ostream& operator<<(std::ostream& os, const Point& point) {
	os << "(" << point.x << ", " << point.y << ")" << endl;
	return os;
}

//ca 叉乘 cb  x1y2 - x2y1
LD dis(const Point& c, const Point& a, const Point& b) {
	return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y);
}

// 计算两个向量的叉积
int cross_product(const Point& p1, const Point& p2) {
	LD temp = p1.x * p2.y - p2.x * p1.y;
	if (temp < 0) return -1;
	else if (temp > 0)return 1;
	else return 0;
}

// 计算AB和点C的关系
int cross_product(const Point& a, const Point& b, const Point& c) {
	return cross_product(Point(b.x - a.x, b.y - a.y), Point(c.x - b.x, c.y - b.y));
}

//提取凸包上只在端点的值
vector<Point> points_inline(vector<Point>& p) {
	vector<Point> ans;
	ans.push_back(p[0]);
	Point p0 = p[0];
	Point p1 = p[1];
	for (int i = 2; i < p.size(); i++) {
		Point& p2 = p[i];
		if (cross_product(p0, p1, p2) == IN) {
			p1 = p2;
		}
		else {
			ans.push_back(p1);
			p0 = p1;
			p1 = p2;
		}
	}
	ans.push_back(p1);
	return ans;
}

//Points中的点x从小到大，x相同则y从小到大
bool my_sort(const Point& a, const Point& b) {
	if (a.real() == b.real()) return a.imag() < b.imag();
	else return a.real() < b.real();
}

vector<Point> insert_hull(int index) {
	//输入一个index，返回0-index 的元素的凸包，输出从x最小的点，顺时针旋转输出
	vector<Point> ans;
	//3个点
	if (index <= 2) {
		Point a = Points[0];
		Point b = Points[1];
		Point c = Points[2];
		ans.push_back(a);
		if (cross_product(a, b, c) == LEFT) {
			ans.push_back(c);
			ans.push_back(b);
		}
		else {//包括了在一条直线上的情况，如果在一条直线上，直接按原来排好的序。
			ans.push_back(b);
			ans.push_back(c);
		}
	}
	else {
		vector<Point> temp = insert_hull(index - 1);
		//找左右支撑线
		//在小凸包上找，即temp
		Point q = Points[index];
		int tot = temp.size();
		Point left, right;
		bool if_left=0, if_right=0;
		//从最近的点开始
		for (int i = tot - 1; i >= 0; i--) {
			Point p = temp[i];
			Point p1 = temp[(i - 1 + tot) % (tot)];
			Point p2 = temp[(i + 1 ) % (tot)];
			//都在右边
			if (cross_product(q, p, p1) != LEFT && cross_product(q, p, p2) != LEFT) {
				left = p;
				if_left = 1;
			}
			else if (cross_product(q, p, p1) != RIGHT && cross_product(q, p, p2) != RIGHT) {
				right = p;
				if_right = 1;
			}
			if (if_left && if_right) break;
		}

		//根据支撑线的点找到
		bool if_add = 1;
		for (auto t : temp) {
			if (if_add) ans.push_back(t);
			if (t == right) {
				if_add = 0;
				ans.push_back(q);
			}
			if (!if_add &&t == left) {
				if_add = 1;
				ans.push_back(t);
			}
		}

	}
	return ans;
}

vector<Point> merge_hull(int left,int right) {
	//返回[left,right]元素构成的凸包
	vector<Point> ans;
	int len = right - left + 1;
	if (len <= 3) {
		if (len == 2) {
			ans.push_back(Points[left]);
			ans.push_back(Points[left + 1]);
		}
		else {
			Point a = Points[left + 0];
			Point b = Points[left + 1];
			Point c = Points[left + 2];
			ans.push_back(a);
			if (cross_product(a, b, c) == LEFT) {
				ans.push_back(c);
				ans.push_back(b);
			}
			else {//包括了在一条直线上的情况，如果在一条直线上，直接按原来排好的序。
				ans.push_back(b);
				ans.push_back(c);
			}
		}
	}
	else {
		vector<Point> temp1 = merge_hull(left, left + len / 2 - 1);
		vector<Point> temp2 = merge_hull(left + len / 2, right);
		//cout << left << endl << left + len / 2 - 1 << endl << right << endl;
	//找到上桥和下桥
		auto ita = max_element(temp1.begin(), temp1.end(), [](const Point& p1, const Point& p2) {
			return p1.x < p2.x;
			});
		int posa = distance(temp1.begin(), ita);
		Point a = *ita;
		auto itb = min_element(temp2.begin(), temp2.end(), [](const Point& p1, const Point& p2) {
			return p1.x < p2.x;
			});
		int posb = distance(temp2.begin(), itb);
		Point b = *itb;
		//cout << a << b << endl;
		int len1 = temp1.size();
		int len2 = temp2.size();

		Point upa = a;
		Point upb = b;
		Point downa = a;
		Point downb = b;
	//上桥
		while (1) {
			bool if_left = 0;
			bool if_right = 0;
			//判断是否为a的左支撑线
			{
				Point a1 = temp1[(posa - 1 + len1) % len1];
				Point a2 = temp1[(posa + 1) % len1];
				if (cross_product(upa, upb, a1) != LEFT && cross_product(upa, upb, a2) != LEFT) {
					if_left = 1;
				}
				//不是左支撑线
				else {
					//找到左支撑线
					for (int i = len1 - 1; i >= 0; i--) {
						Point p = temp1[i];
						Point p1 = temp1[(i - 1 + len1) % (len1)];
						Point p2 = temp1[(i + 1) % (len1)];
						//都在右边
						if (cross_product(p, upb, p1) != LEFT && cross_product(p, upb, p2) != LEFT) {
							upa = p;
							break;
						}
					}
				}
			}
			//判断是否为b的左支撑线(或者右)
			{
				{
					Point a1 = temp2[(posb - 1 + len2) % len2];
					Point a2 = temp2[(posb + 1) % len2];
					if (cross_product(upa, upb, a1) != LEFT && cross_product(upa, upb, a2) != LEFT) {
						if_right = 1;
					}
					//不是左支撑线
					else {
						//找到左支撑线
						for (int i = len2 - 1; i >= 0; i--) {
							Point p = temp2[i];
							Point p1 = temp2[(i - 1 + len2) % (len2)];
							Point p2 = temp2[(i + 1) % (len2)];
							//都在右边
							if (cross_product(upa, p, p1) != LEFT && cross_product(upa, p, p2) != LEFT) {
								upb = p;
								break;
							}
						}
					}
				}
			
			}
			if (if_left && if_right) break;
		}
	//下桥
		while (1) {
			bool if_left = 0;
			bool if_right = 0;
			//判断是否为downa的左支撑线
			{
				Point a1 = temp1[(posa - 1 + len1) % len1];
				Point a2 = temp1[(posa + 1) % len1];
				if (cross_product(downa, downb, a1) != RIGHT && cross_product(downa, downb, a2) != RIGHT) {
					if_left = 1;
				}
				//不是左支撑线
				else {
					//找到左支撑线
					for (int i = len1 - 1; i >= 0; i--) {
						Point p = temp1[i];
						Point p1 = temp1[(i - 1 + len1) % (len1)];
						Point p2 = temp1[(i + 1) % (len1)];
						//都在右边
						if (cross_product(p, downb, p1) != RIGHT && cross_product(p, downb, p2) != RIGHT) {
							downa = p;
							break;
						}
					}
				}
			}
			//判断是否为downb的左支撑线(或者右)
			{
				{
					Point a1 = temp2[(posb - 1 + len2) % len2];
					Point a2 = temp2[(posb + 1) % len2];
					if (cross_product(downa, downb, a1) != RIGHT && cross_product(downa, downb, a2) != RIGHT) {
						if_right = 1;
					}
					//不是左支撑线
					else {
						//找到左支撑线
						for (int i = len2 - 1; i >= 0; i--) {
							Point p = temp2[i];
							Point p1 = temp2[(i - 1 + len2) % (len2)];
							Point p2 = temp2[(i + 1) % (len2)];
							//都在右边
							if (cross_product(downa, p, p1) != RIGHT && cross_product(downa, p, p2) != RIGHT) {
								downb = p;
								break;
							}
						}
					}
				}

			}
			if (if_left && if_right) break;
		}

	//拼接
		int indexa = 0, indexb = 0;
		while (1) {
			//加入a初始
			Point& aa = temp1[indexa];
			if (aa != upa) {
				ans.push_back(aa);
				indexa++;
			}
			else {
				//加入b中的
				//找到upb
				ans.push_back(aa);
				bool if_add = 0;
				bool if_start = 0;
				int i = 0;
				while (1) {
					if (temp2[i] == upb) {
						while (1) {
							ans.push_back(temp2[i]);
							i++;
							if (i == len2) i = 0;
							if (temp2[i] == downb) {
								ans.push_back(downb);
								break;
							}
						}
						break;
					}
					i++;
				}
				//加入a剩余
				if_add = 0;
				while (indexa < len1) {
					if (temp1[indexa] == downa) if_add = 1;
					if (if_add) ans.push_back(temp1[indexa]);
					indexa++;
				}
				break;
			}
		}
	}


	return ans;
}

list<Point> quick_half_hull(list<Point> S, Point b, Point a) {
	if (S.size() == 0) return S;
	else {
		//求最远点
		Point c = *S.begin();
		LD d = dis(c, a, b);
		for (auto s : S) {
			if (dis(s, a, b) > d) {
				d = dis(s, a, b);
				c = s;
			}
		}
		//print_list("S",S);
		//cout << c;
		list<Point> A, B;
		for (auto s : S) {
			if (cross_product(b, c, s) == RIGHT) A.push_back(s);
			else if (cross_product(c, a, s) == RIGHT) B.push_back(s);
		}
		//print_list("A", A);
		//print_list("B", B);
		list<Point> QA = quick_half_hull(A, b, c);
		list<Point> QB = quick_half_hull(B, c, a);
		//print_list("QA", QA);
		//print_list("QB", QB);
		QB.push_back(c);
		QB.splice(QB.end(), QA);
		return QB;
	}
}

list<Point> quick_hull(vector<Point>& points) {
	Point a = points[0];
	Point b = points[points.size() - 1];
	//cout << a << b << endl;
	list<Point> A, B;
	for (int i = 1; i < points.size() - 1; i++) {
		if (cross_product(b, a, points[i]) == RIGHT) A.push_back(points[i]);
		else if (cross_product(a, b, points[i]) == RIGHT) B.push_back(points[i]);
	}
	//print_list("A", A);
	//print_list("B", B);
	list<Point> QA = quick_half_hull(A, b, a);
	list<Point> QB = quick_half_hull(B, a, b);
	//print_list("QA",QA);
	//print_list("QB", QB);
	QA.push_front(a);
	QA.push_back(b);
	QA.splice(QA.end(), QB);
	return QA;
}

void init() {
#define I(x,y) Points[cnt++] = Point({x,y}); 
	int cnt = 0;
	I(0,2) I(3,1) I(8,0) I(2,3) I(1,5) I(2,8) I(4,9) I(6,7) I(5,6) I(9,4) I(7,3)
}

void test() {
	int cnt = 0;
	for (auto P : Points) {
		if (P.real()+P.imag() != 0) {
			printf("Point:%d \tx:%.3Lf\t\ty:%.3Lf\n", ++cnt, P.real(), P.imag());
		}
	}
}

int main() {
	init();
	//test(); //输出点
	//排序
	sort(Points, Points + POINTSNUM, my_sort);
	//点数
	int testnum = 11;
	vector<Point> v;
	for (int i = 0; i < POINTSNUM; i++) v.push_back(Points[i]);
	vector<Point> ans;
	ans = insert_hull(testnum-1);
	vector<Point> ans1 = points_inline(ans);
	ans = merge_hull(0, testnum - 1);
	vector<Point> ans2 = points_inline(ans);
	
	list<Point> ans3 = quick_hull(v);
	cout << "ans1" << endl;
	for (auto a : ans1) {
		cout << a;
	}
	cout << "ans2" << endl;
	for (auto a : ans2) {
		cout << a;
	}
	cout << "ans3" << endl;
	for (auto a : ans3) {
		cout << a;
	}
	return 0;
}

//需要解释的是：
//
//一开始使用了自己的框架，所以前两个函数定义如下：
//
//vector<Point> insert_hull(int index)
//
//vector<Point> merge_hull(int left, int right)
//
//使用了一个全局数组Points存放相关点,
//
//index代表计算Points[0 - index]的元素的凸包
//
//left和right代表计算Points[left - right]的元素的凸包
//
//第三个函数使用了原本的框架
//
//因为写完前两个函数才发现这个问题，且这不影响代码实现逻辑，只影响了元素的访问和存取（实际上后来我才发现用list可以很方便的访问前后元素，相关代码就可以简化），所以并没有修改。

