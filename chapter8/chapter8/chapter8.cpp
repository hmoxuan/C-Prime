// chapter8.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <string>

// 8.1 -- san inline function definition
inline double square(double x) { return x * x;}
void inlineFunc(void)
{
	using namespace std;
	double a, b;
	double c = 13.0;

	a = square(5.0);
	b = square(4.5 + 7.5);
	cout << "a = " << a << ", b = " << b << endl;
	cout << "c = " << c;
	cout << ", c squared = " << square(c++) << endl;
	cout << "Now c = " << c << endl;
}

// 8.2 firstref -- defining and using a reference
void firstref(void)
{
	using namespace std;
	int rats = 101;
	int & rodents = rats;
	cout << "rats = " << rats;
	cout << ", rodents = " << rodents << endl;
	rodents++;
	cout << "rats = " << rats;
	cout << ", rodents = " << rodents << endl;
}

// 8.3 sceref -- defining and using a reference
void sceref(void)
{
	using namespace std;
	int rats = 101;
	int & rodents = rats;
	cout << "rats = " << rats;
	cout << ", rodents = " << rodents << endl;

	cout << "rats address = " << &rats;
	cout << ", rodents address = " << &rodents << endl;

	int bunnies = 50;
	rodents = bunnies;
	cout << "bunnies = " << bunnies;
	cout << ", rats = " << rats;
	cout << ", rodents = " << rodents << endl;

	cout << "bunnies address = " << &bunnies;
	cout << ", rodents address = " << &rodents << endl;
}

// 8.4 swaps -- waapping with references and with pointers
void swapr(int & a, int & b); // a, b are aliases for ints
void swapp(int * p, int * q); // p, q are aliases for ints
void swapv(int a, int b); // a, b are aliases for ints

void swaping(void)
{
	using namespace std;
	int wallet1 = 300;
	int wallet2 = 350;

	cout << "wallet1 = $" << wallet1;
	cout << "wallet2 = $" << wallet2 << endl;

	cout << "Using references to swap contents:\n";
	swapr(wallet1, wallet2); // pass variables
	cout << "wallet1 = $" << wallet1;
	cout << "wallet2 = $" << wallet2 << endl;

	cout << "Using pointers to swap contents again:\n";
	swapp(&wallet1, &wallet2); // pass variables
	cout << "wallet1 = $" << wallet1;
	cout << "wallet2 = $" << wallet2 << endl;

	cout << "Trying to use passing by balue:\n";
	swapv(wallet1, wallet2); // pass variables
	cout << "wallet1 = $" << wallet1;
	cout << "wallet2 = $" << wallet2 << endl;
}

// a, b are aliases for ints
void swapr(int & a, int & b)
{
	int temp;
	temp = a;
	a = b;
	b = temp;
}

// p, q are aliases for ints
void swapp(int * p, int * q)
{
	int temp;
	temp = *p;
	*p = *q;
	*q = temp;
}

// a, b are aliases for ints
void swapv(int a, int b)
{
	int temp;
	temp = a;
	a = b;
	b = temp;
}

// 8.5 cubes -- regular and references arguments
double cube(double a)
{
	a *= a * a;
	return a;
}
double refcube(double &ra)
{
	ra *= ra * ra;
	return ra;
}
void cubesFunc(void)
{
	using namespace std;
	double x = 3.0;
	cout << cube(x) << " = cube of " << x << endl;
	cout << refcube(x) << " = cube of " << x << endl;
}

// 8.6 strtref -- using structure references
struct free_throws
{
	std::string name;
	int made;
	int attempts;
	float precent;
};

void display(const free_throws & ft)
{
	using namespace std;
	cout << "  Made: " << ft.name << '\n';
	cout << "  Made: " << ft.made << '\t';
	cout << "Attempts: " << ft.attempts << '\t';
	cout << "Percent: " << ft.precent << '\n';
}
void set_pc(free_throws & ft)
{
	if (ft.attempts != 0)
	{
		ft.precent = 100.0f *float(ft.made) / float(ft.attempts);	
	}
	else
	{
		ft.precent = 0;
	}
}
free_throws & accumulate(free_throws & target, const free_throws & source)
{
	target.attempts += source.attempts;
	target.made += source.made;
	set_pc(target);
	return target;
}
void strtrefFunc(void)
{
	using namespace std;
	free_throws one = {"Ifelsa Branch", 13, 14};
	free_throws two = {"Andor Knoot", 10, 16};
	free_throws three = {"Minnie Max", 7, 9};
	free_throws four = {"Whily Looper", 5, 9};
	free_throws five = {"Long Long", 6, 14};
	free_throws team = {"Throwgoods", 0, 0};
	free_throws dup;

	set_pc(one);
	display(one);
	accumulate(team, one);
	display(team);

	// use return value as argument
	display( accumulate(team, two) );
	accumulate( accumulate(team, three) , four);
	display(team);

	// use return value as assignment
	dup = accumulate(team, five);
	cout << "Displaying team:\n";
	display(team);
	cout << "Displaying dup after assignment:\n";
	display(dup);
	set_pc(four);

	// ill-advised assignment
	accumulate(dup, five) = four;
	cout << "Displaying dup after  ill-advised assignment:\n";
	display(dup);
}

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	cout << "chapter8" << endl;
	//inlineFunc();
	//firstref();
	//sceref();
	//swaping();
	//cubesFunc();
	strtrefFunc();

	system("pause");
	return 0;
}


