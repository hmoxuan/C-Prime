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

// 5.17 textin2 -- using cin.get(char)
void textin2(void)
{
	using namespace std;
	char ch;
	int count = 0;

	cout <<"Enter characters; enter # to quilt: " << endl;
	cin.get(ch);
	while (ch != '#')
	{
		cout << ch;
		++count;
		cin.get(ch); // use it again
	}
	cout << endl << count << "characters read" << endl;
}

// 5.18 textin3 -- reading chars to end of file
void textin3(void)
{
	using namespace std;
	char ch;
	int count = 0;

	cin.get(ch); // attempt to read a char
	while (cin.fail() == false) // test for EOF
	{
		cout << ch; // echo character
		++count;
		cin.get(ch); // use it again
	}
	cout << endl << count << "characters read" << endl;
}

// 5.19 textin4 -- reading chars with cin.get()
void textin4(void)
{
	using namespace std;
	char ch;
	int count = 0;

	while ((ch = cin.get()) != false) // test for end-of-file
	{
		cout.put( char(ch) ); // use it again
		++count;
	}
	cout << endl << count << "characters read" << endl;
}

// 5.20 nested -- nested loops and 2-D array
const int Cities = 5;
const int Years = 4;

void nested(void)
{
	using namespace std;
	const char * cities[Cities] = // array of pointers to 5 strings
	{
		"Gribble City",
		"Gribbletown",
		"New Gribble",
		"San Gribble",
		"Gribble Vista"
	};

	int maxtemps[Years][Cities] =
	{
		{96, 100, 87, 101, 105}, // values for maxtemps[0]
		{96, 98, 91, 107, 104}, // values for maxtemps[1]
		{97, 101, 93, 108, 107}, // values for maxtemps[2]
		{98, 103, 95, 109, 108}, // values for maxtemps[3]
	};

	cout << "Maximum temperatures for 2008 - 2011 \n\n";
	for (int city = 0; city < Cities; ++city)
	{
		cout << cities[city] << " :\t ";
		for (int year = 0; year < Years; ++year)
		{
			cout << maxtemps[year][city] << " :\t ";
		}
		cout << endl;
	}
}