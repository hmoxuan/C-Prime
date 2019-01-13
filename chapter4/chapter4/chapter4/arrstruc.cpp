// arrstruc.h : Code 4.13 -- an array of structures
//
#include "stdafx.h"
#include <iostream>
#include "arrstruc.h"

struct inflatable  // structure declaration
{
	char name[20];
	float volume;
	double price;
};

void arrstruc(void)
{
	using namespace std;
	inflatable guests[2] =
	{
		{"Bambi", 0.5, 21.29},
		{"Godzilla", 2000, 565.99},
	}; 
	inflatable choice; 

	cout << "The guests " << guests[0].name << " and " << guests[1].name 
		<< "\n have a combined volume of "
		<< guests[0].volume + guests[1].volume << " cubic feet." << endl;
}


