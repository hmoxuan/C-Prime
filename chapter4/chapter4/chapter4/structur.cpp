// structur.c : Code 4.11 -- a simple structure
//
#include "stdafx.h"
#include <iostream>
#include <cstring>  // declare strlen(), strcpy()
#include "structur.h"

#pragma warning(disable:4996)  //if you want use strcpy function

struct inflatable  // structure declaration
{
	char name[20];
	float volume;
	double price;
};

char * getname(void);

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

// 4.21 -- using new with a structure
void newstruct(void)
{
	using namespace std;
	inflatable *ps = new inflatable; // allot memory for structure

	cout << "Expand name of inflatable item: ";
	cin.get(ps->name, 20); //method 1 for member access
	cout << "Expand volume of cubic feet: ";
	cin >> (*ps).volume; //method 2 for member access
	cout << "Enter price: $";
	cin >> ps->price;

	cout << "Name: " << (*ps).name << endl;
	cout << "Volume: " << ps->volume << endl;
	cout << "Price: " << ps->price << endl;

    delete ps;  // free memory used by structure
}

// 4.22 -- using new delete operator
char * getname(void)
{
	using namespace std;
	char temp[80]; // temporary storage
	cout << "Enter last name : ";
	cin >> temp;
	char * pn = new char[strlen(temp) + 1];
	strcpy(pn , temp);// copy string into smaller space

	return pn;  // temp lost when function ends
}

void deleteOperator(void)
{
	using namespace std;
	char * name; // create pointer but no storage
	name = getname();
	cout << name << " at " << (int *) name << endl;
	delete [] name; // memory freed

	name = getname();  //reuse freed memory
	cout << name << " at " << (int *) name << endl;
	delete [] name; // memory freed again

}
