// instr2.h : Code 4.4 reading more than one word with getline
//
#include "stdafx.h"
#include <iostream>

#include "instr2.h"

void instr2(void)
{
	using namespace std;
	const int ArSize = 20;
	char name[ArSize];  // empy array
	char dessert[ArSize];
	
	cout << "Enter your name:\n";
	cin.getline(name, ArSize);  // reads through newline
	cout << "Enter your favorite dessert:\n";
	cin.getline(dessert, ArSize);
    cout << "I have some delicious " << dessert;
	cout << "for you, " << name << ".\n";
}