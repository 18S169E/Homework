#define _CRT_SECURE_NO_WARNINGS
#include "AbstractMachine.h"


using namespace std;

int main() {
	AbstractMachine m;
	m.getCode("asm.txt");
	//m.print_code();

	m.run();
	//m.check_result();

	return 0;
}