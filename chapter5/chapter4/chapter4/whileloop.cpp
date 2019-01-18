// whileloop.cpp : Code for while Loop
//
#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstring>
#include "whileloop.h"

const int ArSize = 20;

// code 5.13 while -- introducing the while loop
void whileloop(void)
{
	using namespace std;
	char name[ArSize];

	cout << "Your first name, please: ";
	cin >> name;
	cout << "Here is your name, verticalized and ASCIIized:\n";
	int i = 0; // start at beginning of string
	while (name[i] != '\0') // process to end of string
	{
		cout << name[i] << ": " << int(name[i]) << endl;
		i++;
	}
}