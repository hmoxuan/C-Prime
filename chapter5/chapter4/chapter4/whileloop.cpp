// whileloop.cpp : Code for while Loop
//
#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstring>
#include <ctime> // describes clock() function, clock_t type
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

// 5.14 code -- using clock() in a time-delay loop
void waiting(void)
{
	using namespace std;
	cout << "Enter the delay time, in seconds: ";
	float secs;
	cin >> secs;
	clock_t delay = secs * CLOCKS_PER_SEC; // convert to clock ticks
	cout << "starting \a\n";
	clock_t start = clock();
	while ( clock() - start < delay ); // wait until time elapses note the semicolon
	cout << "done \a\n";
}

// 5.15 do while -- exit-condition loop
void dowhile(void)
{
	using namespace std;
	int n;

	cout << "Enter numbers in the range 1-10 to find ";
	cout << " my favorite number.\n";
	do
	{
		cin >> n; // execute body
	} while( n != 7); // then test
	cout << "Yes, 7 is my favorite.\n";
}

// 5.16 textin1 -- reading chars with a while loop
void textin1(void)
{
	using namespace std;
	char ch;
	int count = 0; // use basic input
	cout << "Enter characters; enter # to quit: \n";
	cin >> ch; // get a character
	while ( ch != '#')
	{
		cout << ch;
		++ count;
		cin >> ch;
	}

	cout << endl << count << " characters read.\n";
}

