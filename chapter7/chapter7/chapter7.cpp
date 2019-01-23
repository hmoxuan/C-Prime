// chapter7.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <string>
#include <cmath>
#include <array>

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
 * code 7.8 arrfun4 -- functions with an array argument
 *
 /****************************************************/
// return the sum of an integer array
int sum_arr4(const int * begin, const int * end)
{
	const int * pt;
	int total = 0;

	for (pt = begin; pt != end; pt++)
	{
		total = total + *pt;
	}
	return total;
}

void arrfun4(void)
{
	using namespace std;
	cout << "ArSize = " << ArSize;
	int cookies[ArSize] = {1, 2, 4, 8, 16, 32, 64, 128};

	int sum = sum_arr4(cookies, cookies + ArSize);
	cout << "Total cookies eaten: " << sum << endl;

	sum = sum_arr4(cookies, cookies + 3); // first 3 elements
	cout << "First three eaters ate : " << sum << " cookies." << endl;

	sum = sum_arr4(cookies + 4, cookies + 8); // last 4 elements
	cout << "Last four eaters ate : " << sum << " cookies." << endl;
}

/******************************************************
 * code 7.9 strgfun -- functions with a string argument
 *
 /****************************************************/
unsigned int c_in_str(const char * str, char ch)
{
	unsigned int count = 0;
	while (*str)
	{
		if (*str == ch)
		{
			count++;
		}
		str++;
	}
	return count;
}

void strgfun(void)
{
	using namespace std;
	char mmm[15] = "minimum"; // string in an array
	char *wail = "ululate";
	unsigned int ms = c_in_str(mmm, 'm');
	unsigned int us = c_in_str(mmm, 'u');

	cout << ms << " m characters in " << mmm << endl;
	cout << us << " u characters in " << wail << endl;
}

/******************************************************
 * code 7.10 strgback -- a functions that returns a pointer to char
 *
 /****************************************************/
char * buildstr(char c, int n)
{
	char * pstr = new char[n+1];
	pstr[n] = '\0';
	while (n-- > 0)
	{
		pstr[n] = c;
	}
	return pstr;
}

void strgback(void)
{
	using namespace std;
	int times;
	char ch;

	cout << "Enter a character: ";
	cin >> ch;
	cout << "Enter an integer: ";
	cin >> times;

	char *ps = buildstr(ch, times);
	cout << ps << endl;
	delete [] ps;  // free memory

	ps = buildstr('+', 20); // reuse pointer
	cout << ps << "-DONE-" << ps << endl;
	delete [] ps;  // free memory
}

/******************************************************
 * code 7.11 travel -- using structure with functions
 *
 /****************************************************/
struct travel_time
{
	int hours;
	int mins;
};

const int Mins_per_hr = 60;
travel_time sum(travel_time t1, travel_time t2)
{
	travel_time total;
	total.mins = (t1.mins + t2.mins) % Mins_per_hr;
	total.hours = t1.hours + t2.hours + (t1.hours + t2.hours) / Mins_per_hr;
	return total;
}

void show_time(travel_time t)
{
	using namespace std;
	cout << t.hours << " hours, " << t.mins << " minutes\n";

}

void travel(void)
{
	using namespace std;
	travel_time day1 = {5, 45};
	travel_time day2 = {4, 55};

	travel_time trip = sum(day1, day2);
	cout << "Two-day total: ";
	show_time(trip);

	travel_time day3 = {4, 32};
	cout << "Three-day total: ";
	show_time( sum(trip, day3) );
}

/******************************************************
 * code 7.12 atrctfun -- functions with a structure argument 
 *
 /****************************************************/
// structure declarations
struct polar
{
	double distance;
	double angle;
};

struct rect
{
	double x;
	double y;
};

// convert rectangular to polar coordinates
polar rect_to_polar(rect xypos)
{
	using namespace std;
	polar answer;
	answer.distance = sqrt( xypos.x * xypos.x  + xypos.y * xypos.y);
	answer.angle = atan2(xypos.y, xypos.x);

	return answer;
}

// show polar coordinates, converting angle to degrees
void show_polar (polar dapos)
{
	using namespace std;
	const double Rad_to_dag = 57.29577951;

	cout << "distance = " << dapos.distance;
	cout << ", angle = " << dapos.angle * Rad_to_dag;
	cout << " degrees \n";
}

void atrctfun(void)
{
	using namespace std;
	rect rplace;
	polar pplace;

	cout << "Enter the x and y values: ";
	while( cin >> rplace.x >> rplace.y)
	{
		pplace = rect_to_polar(rplace);
		show_polar(pplace);
		cout << "Next two numbers (q to quit): ";
	}
	cout << "Done.\n";
}

/******************************************************
 * code 7.13 strctptr -- functions with a pointer to structure argument 
 *
 /****************************************************/
// convert rectangular to polar coordinates
void rect_to_polar2(const rect * pxy, polar * pda)
{
	using namespace std;
	pda->distance = sqrt( pxy->x * pxy->x  + pxy->y * pxy->y);
	pda->angle = atan2(pxy->y, pxy->x);
}

// show polar coordinates, converting angle to degrees
void show_polar2 (const polar * pda)
{
	using namespace std;
	const double Rad_to_dag = 57.29577951;

	cout << "distance = " << pda->distance;
	cout << ", angle = " << pda->angle * Rad_to_dag;
	cout << " degrees \n";
}

void strctptr(void)
{
	using namespace std;
	rect rplace;
	polar pplace;

	cout << "Enter the x and y values: ";
	while( cin >> rplace.x >> rplace.y)
	{
		rect_to_polar2(&rplace, &pplace);
		show_polar2(&pplace);
		cout << "Next two numbers (q to quit): ";
	}
	cout << "Done.\n";
}

/******************************************************
 * code 7.14 topfive -- handling an array of string objects 
 *
 /****************************************************/
void display(const std::string sa[], int n)
{
	using namespace std;
	for (int i = 0; i < n; i++)
		cout << i + 1 << ": " << sa[i] << endl;
}

void topfive(void)
{
	using namespace std;
	const int SIZE = 5;
	string list[SIZE]; // an array holding 5 string object
	cout << "Enter your " << SIZE << " favorite astronomical sights:\n";
	for (int i = 0; i < SIZE; i++)
	{
		cout << i + 1 << ": ";
		getline(cin, list[i]);
	}

	cout << "Your list: \n";
	display(list, SIZE);
}

/******************************************************
 * code 7.15 arrobj -- functions with array objects (C++)
 *
 /****************************************************/
const int Seasons = 4;
const std::array<std::string, Seasons> Snames = {"Spring", "Summer", "Fall", "Winter"};

void fill_15(std::array<double, Seasons> * pa)
{
	using namespace std;
	for (int i = 0; i < Seasons; i++)
	{
		cout << "Enter " << Snames[i] << "expenses: ";
		cin >> (*pa)[i];
	}
}

void show_15(std::array<double, Seasons> da)
{
	using namespace std;
	double total = 0.0;
	cout << "\nEXPENSES\n";
	for (int i = 0; i< Seasons; i++)
	{
		cout << Snames[i] << ": $" << da[i] << endl;
		total += da[i];
	}
	cout << "Total Expenses: $" << total << endl;
}

void arrobj(void)
{
	std::array<double, Seasons> expenses;
	fill_15(&expenses);
	show_15(expenses);
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
	//arrfun3();
	//arrfun4();
	//strgfun();
	//strgback();
	//travel();
	//atrctfun();
	//strctptr();
	arrobj();

	system("pause");
	return 0;
}

