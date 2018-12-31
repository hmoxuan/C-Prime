// instr3.h : Code 4.5 reading more than one word with getline
//
#include "stdafx.h"
#include <iostream>

#include "instr3.h"

void instr3(void)
{
	using namespace std;
	const int ArSize = 20;
	char name[ArSize];  // empy array
	char dessert[ArSize];
	
	cout << "Enter your name:\n";
	cin.get(name, ArSize).get();  // reads string, newline
	cout << "Enter your favorite dessert:\n";
	cin.get(dessert, ArSize).get();
    cout << "I have some delicious " << dessert;
	cout << "for you, " << name << ".\n";
}