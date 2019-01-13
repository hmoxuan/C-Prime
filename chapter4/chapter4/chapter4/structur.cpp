// structur.c : Code 4.11 -- a simple structure
//
#include "stdafx.h"
#include <iostream>
#include "structur.h"

struct inflatable  // structure declaration
{
	char name[20];
	float volume;
	double price;
};

void structur(void)
{
	using namespace std;
	inflatable guest =
	{
		"Glorious Gloria",  // name value
		1.88,  // volume value
		29.99  // price value
	};  // guest is a structure variable of type inflatable

	// It's initialize to the indicated values
	inflatable pal =
	{
		"Audacious Arthur",  // name value
		3.12,  // volume value
		32.99  // price value
	};  // pal is a structure variable of type inflatable

	cout << "Expand your guest list with " << guest.name;
	cout << " and " << pal.name << "!\n";
	cout << "You can have both for $";
	cout << guest.price + pal.price << "!\n";
}


