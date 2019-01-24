// chapter8.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>

// 8.1 -- san inline function definition
inline double square(double x) { return x * x;}
void inlineFunc(void)
{
	using namespace std;
	double a, b;
	double c = 13.0;

	a = square(5.0);
	b = square(4.5 + 7.5);
	cout << "a = " << a << ", b = " << b << endl;
	cout << "c = " << c;
	cout << ", c squared = " << square(c++) << endl;
	cout << "Now c = " << c << endl;
}

// 8.2 firstref -- defining and using a reference
void firstref(void)
{
	using namespace std;
	int rats = 101;
	int & rodents = rats;
	cout << "rats = " << rats;
	cout << ", rodents = " << rodents << endl;
	rodents++;
	cout << "rats = " << rats;
	cout << ", rodents = " << rodents << endl;
}

// 8.3 sceref -- defining and using a reference
void sceref(void)
{
	using namespace std;
	int rats = 101;
	int & rodents = rats;
	cout << "rats = " << rats;
	cout << ", rodents = " << rodents << endl;

	cout << "rats address = " << &rats;
	cout << ", rodents address = " << &rodents << endl;

	int bunnies = 50;
	rodents = bunnies;
	cout << "bunnies = " << bunnies;
	cout << ", rats = " << rats;
	cout << ", rodents = " << rodents << endl;

	cout << "bunnies address = " << &bunnies;
	cout << ", rodents address = " << &rodents << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	cout << "chapter8" << endl;
	//inlineFunc();
	//firstref();
	sceref();

	system("pause");
	return 0;
}


