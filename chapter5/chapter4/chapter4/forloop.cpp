// forloop.cpp : Code 5.1 for Loop
//
#include "stdafx.h"
#include <iostream>
#include <string>
#include <cstring>
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

// code 5.4 formore -- more looping with for
const int ArSize = 16;  // example of external declaration
void formore(void)
{
	using namespace std;
	long long factorials[ArSize];
	factorials[1] = factorials[0] = 1LL;

	for ( int i = 2; i < ArSize; i++)
	{
		factorials[i] = i * factorials[i-1];
	}
	
	for ( int i = 0; i < ArSize; i++)
	{
		cout << i << "! = " << factorials[i] << endl;
	}
}

// code 5.5 bigstep -- count as directed
void bigstep(void)
{
	using namespace std;
	cout << "Enter an integer: ";
	int by;
	cin >> by;
	cout << "Counting by " << by << "s:\n";
	for ( int i = 0; i < 100; i = i + by)
	{
		cout << i << endl;
	}
}

// code 5.6 forstr1 -- using for with a string
void forstr1(void)
{
	using namespace std;
	string word;
	cout << "Enter a word: ";
	cin >> word;

	// display letters in reverse order
	for ( int i = word.size() -1; i >= 0; i--)
	{
		cout << word[i];
	}
	cout << "\nBye.\n";
}

// code 5.7 plus_one -- the increment operator
void plus_one(void)
{
	using namespace std;
	int a = 20;
	int b = 20;
	cout << "a = " << a << ":  b = " << b << endl;
	cout << "a++ = " << a++ << ":  ++b = " << ++b << endl;
	cout << "a = " << a << ":  b = " << b << endl;
}

// code 5.8 block -- use a block statement
void block(void)
{
	using namespace std;
	cout << "The Amazing Accounto will sum and average ";
	cout << "five numbers for you." << endl;
	cout << "Please enter five values" << endl;
	double number;
	double sum = 0.0;
	for ( int i = 1; i <= 5; i++)
	{
		cout << "Value " << i << ": ";
		cin >> number;
		sum += number;
	}
	cout << "Five exquisite choices inded! ";
	cout << "They sum to " << sum << endl;
	cout << "and average to " << sum / 5 << endl;
	cout << "The Amazing Accounto bids you adieu" << endl;
}

// code 5.9 forstr2 -- reversing an array
void forstr2(void)
{
	using namespace std;
	string word;
	cout << "Enter a word: ";
	cin >> word;

	// physically modify string object
	char temp;
	int i, j;
	for ( j = 0, i = word.size() -1; j < i; --i, ++j)
	{
		temp = word[i];
		word[i] = word[j];
		word[j] = temp;
	}
	cout << word << "\nDone.\n";
}

// code 5.10 equal -- equality vs assignment
void equal(void)
{
	using namespace std;
	int quizscores[10] = {20, 20, 20, 20, 20, 19, 20, 18, 20, 20};

	cout << "Doing it right: \n";
	int i;
	for ( int i = 0; quizscores[i] == 20; i++)
	{
		cout << "quiz " << i << "is a 20 \n";
	}
/*
	cout << "Doing it dangerously worong: " << endl;
	for ( int i = 0; quizscores[i] = 20; i++)
	{
		cout << "quiz " << i << "is a 20 \n";
	}
*/
}

// code 5.11 compstr1 -- comparing strings using arrays
void compstr1(void)
{
	using namespace std;
	char word[5] = "?ate";
	for (char ch = 'a'; strcmp(word, "mate"); ch++)
	{
		cout << word << endl;
		word[0] = ch;
	}
	cout << "After loop ends, word is " << word << endl;
}

// code 5.12 compstr1s-- comparing strings using arrays
void compstr2(void)
{
	using namespace std;
	string word = "?ate";
	for (char ch = 'a'; word != "mate"; ch++)
	{
		cout << word << endl;
		word[0] = ch;
	}
	cout << "After loop ends, word is " << word << endl;
}