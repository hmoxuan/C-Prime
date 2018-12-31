// numStr.h : Code 4.6 following number input with line input
//
#include "stdafx.h"
#include <iostream>

#include "numStr.h"

void numstr(void)
{
	using namespace std;
	cout << "What year was your hose built?\n";
	int year;
	(cin >> year).get();  // or (cin >> year).get(ch);
	cout << "What is its street address?\n";
	char address[80];
	cin.getline(address, 80);
	cout << "Year built: " << year << endl;
	cout << "Address: " << address << endl;
	cout << "Done!\n";
}
