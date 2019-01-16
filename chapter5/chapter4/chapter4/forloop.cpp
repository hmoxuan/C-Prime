// forloop.cpp : Code 5.1 for Loop
//
#include "stdafx.h"
#include <iostream>
#include "forloop.h"

// code 5.1 forloop -- introducing the for loop
void forloop(void)
{
	using namespace std;
	int i; // create a counter

	// initialize; test; update
	for (i =0; i < 5; i++)
		cout << "C++ knows loops.\n";
	cout << "C++ knows when to stop.\n";
}

// code 5.2 num_test -- use numeric test in for loop
void num_test(void)
{
	using namespace std;
	cout << "Enter the starting countdown value: ";
	int limit;
	cin >> limit;
	int i; // create a counter

	for (i =limit; i; i--)
		cout << "i = " << i << endl;
	cout << "Done now that i = " << i << endl;
}

// code 5.3 express -- value of expressions
void express(void)
{
	using namespace std;
	int x;
	cout << "The expression x = 100 has the value ";
	cout << (x = 100) << endl;
	cout << "Now x = " << x << endl;

	cout << "The expression x < 3 has the value ";
	cout << (x < 3) << endl;
	cout << "The expression x > 3 has the value ";
	cout << (x > 3) << endl;

	cout.setf(ios_base::boolalpha); // a newer C++ feature
	cout << "The expression x < 3 has the value ";
	cout << (x < 3) << endl;
	cout << "The expression x > 3 has the value ";
	cout << (x > 3) << endl;
}