#include "自动机.h"

int main() {


	Machine m;
	m.init("input.txt","output.txt");
	m.run();
	cout << "分析完成" << endl;
	return 0;
}