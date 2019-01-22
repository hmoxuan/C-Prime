// chapter7.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>

/******************************************************
 * code 7.1 calling -- defining, prototyping, and calling a function
 *
 /****************************************************/
void simple(void)
{
	using namespace std;
	cout << "I'm but a simple function. \n";
}

void callingFunc( void)
{
	using namespace std;
	cout << "callingFunc() will call the simple() function. \n";
	simple(); // function call
	cout << "callingFunc() is finished with the simple() function. \n";
}

/******************************************************
 * code 7.2 protos -- using prototypes and function calls
 *
 /****************************************************/
void cheers(int n)
{
	using namespace std;
	for (int i = 0; i < n; i++)
	{
		cout << "Cheers! ";
	}
	cout << endl;
}

double cube(double x)
{
	return x * x * x;
}

void protoes(void)
{
	using namespace std;
	cheers(5);
	cout << "Give me a number: ";
	double side;
	cin >> side;
	double volume = cube(side);

	cout << "A " << side << "-foot cube has a volume of ";
	cout << volume << " cubic feet.\n";
	cheers(cube(2));
}

/******************************************************
 * code 7.3 twoarg -- a function with 2 arguments
 *
 /****************************************************/
void n_chars(char c, int n)
{
    using namespace std;
	while (n-- > 0)
		cout << c;
}

void twoarg(void)
{
	using namespace std;
	int times;
	char ch;
	cout << "Enter a character: ";
	cin >> ch;
	while (ch != 'q') // q to quit
	{
		cout << "Enter an integer: ";
		cin >> times;
		n_chars(ch, times);
		cout << "\n Enter another character or press the"
			"q-key to quit:";
		cin >> ch;
	}
	cout << "The value of times is " << times << ".\n";
	cout << "Bye\n";
}

/******************************************************
 * code 7.4 lotto -- probability of winning
 *
 /****************************************************/
long double probability(unsigned numbers, unsigned picks)
{
    using namespace std;
	long double result = 1.0;
	long double n;
	unsigned p;

	for (n = numbers, p = picks; p > 0; n--, p--)
		result = result * n / p;

    return result;
}

void lotto(void)
{
	using namespace std;
	double total, choices;
	cout << "Enter the total number of choices on the game card and \n"
		"the number of picks allowed: \n";
	while((cin >> total >> choices) && (choices <= total))
	{
		cout << "You have one chance in ";
		cout << probability(total, choices);
		cout << " of winning.\n";
		cout << "Next two numbers (q to quit): ";
	}
	cout << "bye \n";
}

/******************************************************
 * code 7.5 arrfun1 -- functions with an array argument
 *
 /****************************************************/
const int ArSize = 8;

// return the sum of an integer array
int sum_arr(int arr[], int n)
{
    using namespace std;
	int total = 0;
	for (int i = 0; i < n; i++)
	{
		total = total + arr[i];
	}
	return total;
}

void arrfun1(void)
{
	using namespace std;
	int cookies[ArSize] = {1, 2, 4, 8, 16, 32, 64, 128};

	int sum = sum_arr(cookies, ArSize);
	cout << "Total cookies eaten: " << sum << "\n";
}

/******************************************************
 * code 7.6 arrfun2 -- functions with an array argument
 *
 /****************************************************/

// return the sum of an integer array
int sum_arr2(int arr[], int n)
{
    using namespace std;
	int total = 0;
	cout << arr << " = arr, ";
	cout << sizeof arr << " = sizeof arr \n";
	for (int i = 0; i < n; i++)
	{
		total = total + arr[i];
	}
	return total;
}

void arrfun2(void)
{
	using namespace std;
	int cookies[ArSize] = {1, 2, 4, 8, 16, 32, 64, 128};

	cout << cookies << " = array address, ";
	cout << sizeof cookies << " = sizeof cookies\n";
	int sum = sum_arr2(cookies, ArSize);
	cout << "Total cookies eaten: " << sum << endl;

	sum = sum_arr2(cookies, 3); // a lie
	cout << "First three eaters ate : " << sum << " cookies." << endl;

	sum = sum_arr2(cookies + 4, 4); // another lie
	cout << "Last four eaters ate : " << sum << " cookies." << endl;
}

/******************************************************
 * code 7.7 arrfun3 -- array functions and const
 *
 /****************************************************/
const int Max = 5;

int fill_array(double ar[], int limit)
{
	using namespace std;
	double temp;
	int i;
	for (i = 0; i < limit; i++)
	{
		cout << "Enter value #" << (i + 1) << ": ";
		cin >> temp;
		if (!cin) // bad input
		{
			cin.clear();
			while (cin.get() != '\n')
			{
				continue;			
			}
		    cout << "Bad input; input process terminated.\n";
			break;
		}
		else if (temp < 0)
		{
			break;
		}
		ar[i] = temp;
	}
	return i;
}

void show_array(const double ar[], int n)
{
	using namespace std;
	for (int i = 0; i< n; i++)
	{
		cout << "Property #" << (i + 1) << ": $";
		cout << ar[i] << endl;
	}
}

void revalue (double r, double ar[], int n)
{
	for (int i = 0; i< n; i++)
	{
		ar[i] *= r;
	}
}

// return the sum of an integer array
void arrfun3(void)
{
    using namespace std;
	double properties[Max];
	int size = fill_array(properties, Max);

	show_array(properties, size);
	if (size > 0)
	{
		cout << "Enter revaluation factor: ";
		double factor;
		while (!(cin >> factor)) // bad input
		{
			cin.clear();
			while (cin.get() != '\n')
			{
				continue;
			}
			cout << "Bad input; Please enter a number: ";
		}
		revalue(factor, properties, size);
		show_array(properties, size);
	}
	cout << "Done.\n";
}

/******************************************************
 * main function
 *
 /****************************************************/
int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	//callingFunc();
	//protoes();
	//twoarg();
	//lotto();
	//arrfun1();
	//arrfun2();
	arrfun3();

	system("pause");
	return 0;
}

