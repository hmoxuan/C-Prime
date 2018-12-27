// hexoct2.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <climits>

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	int chest = 42;
	int waist = 42;
	int inseam = 42;

	cout << "Monsieur cuts a striking figure!\n";
	cout << "chest = " << chest << " (decimal for 42)" << endl;
	// manipulator for changing number base
	cout << hex;
	cout << "waist = " << waist << " (hexadecimal for 42)" << endl;
	// manipulator for changing number base
	cout << oct;
	cout << "inseam = " << inseam << " (octal for 42)" << endl;

	system("pause");
	return 0;
}


