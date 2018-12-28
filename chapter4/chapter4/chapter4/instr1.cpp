// instr1.h : Code 4.3 reading more than one string
//
#include "stdafx.h"
#include <iostream>

#include "instr1.h"

void instr1(void)
{
	using namespace std;
	const int ArSize = 20;
	char name[ArSize];  // empy array
	char dessert[ArSize];
	
	cout << "Enter your name:\n";
	cin >> name;
	cout << "Enter your favorite dessert:\n";
	cin >> dessert;
    cout << "I have some delicious " << dessert;
	cout << "for you, " << name << ".\n";
}