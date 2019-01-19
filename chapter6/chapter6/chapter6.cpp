// chapter6.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>

// code 6.1. if -- using the if statement
void if_func(void)
{
	using namespace std;
	char ch;
	int spaces = 0;
	int total = 0;
	cin.get(ch);

	while( ch!= '.') // quit at end of sentence
	{
		if (ch == ' ') // check if ch is a space
		{
			++spaces; 
		}
		++total; // done every time
		cin.get(ch);
	}
	cout << spaces << " spaces, " << total;
	cout << " characters total in sentence" << endl;
}

// code 6.2. ifelse -- using the if else statement
void ifelse(void)
{
	using namespace std;
	char ch;
    cout << "Type, and I shall repeat." << endl;
	cin.get(ch);

	while( ch!= '.') // quit at end of sentence
	{
		if (ch == '\n') // check if ch is a space
		{
			cout << ch; 
		}
		else
		{
			cout << ++ch;
		}
		cin.get(ch);
	}
	cout << " \n Please excuse the slight confusion." << endl;
}

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	cout << "chapter6" << endl;
	//if_func();
	ifelse();

	system("pause");
	return 0;
}
