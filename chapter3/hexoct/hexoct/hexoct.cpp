// hexoct.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <climits>

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	int chest = 42;	//decimal integer literal
	int waist = 0x42; //hexadecimal integer literal
	int inseam =042; //octal integer literal

	cout << "Monsieur cuts a striking figure!\n";
	cout << "chest = " << chest << "(42 in decimal)\n";
	cout << "waist = " << waist << "(0x42 in hex)\n";
	cout << "inseam = " << inseam << " (042 in octal)\n";

	system("pause");
	return 0;
}

