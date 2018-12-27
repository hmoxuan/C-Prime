// chartype.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <climits>


int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	char ch;

	cout << "Enter a character: " << endl;
	cin >> ch;
	cout << "Hola! ";
	cout << "Thank you for the " << ch << " character." << endl;


	system("pause");
	return 0;
}

