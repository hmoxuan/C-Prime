// divide.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	cout.setf(ios_base::fixed, ios_base::floatfield);

	cout << "Integer division : 9/5 = " << 9 / 5 << endl;

	cout << "Floating-point division: 9.0/5.0 = ";
	cout << 9.0 / 5.0 <<endl;

    cout << "Mixed division: 9.0/5 = " << 9.0 / 5 <<endl;

	cout << "double consants: 1e7/9.0 = ";
	cout << 1.e7 / 9.0 <<endl;

	cout << "float consants: 1e7/9.0f = ";
	cout << 1.e7f / 9.0f <<endl;

	system("pause");
	return 0;
}

