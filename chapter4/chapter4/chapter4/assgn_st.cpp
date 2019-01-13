// assgn_st.c : Code 4.12 -- assigning structures
//
#include "stdafx.h"
#include <iostream>
#include "assgn_st.h"

struct inflatable  // structure declaration
{
	char name[20];
	float volume;
	double price;
};

void assgn_st(void)
{
	using namespace std;
	inflatable bouquet =
	{
		"sunflowers",  // name value
		0.2,  // volume value
		12.49  // price value
	};  // bouquet is a structure variable of type inflatable
	inflatable choice; 

	cout << "bouquet: " << bouquet.name << " for $";
	cout << bouquet.price << endl;

	// assign one structure to another
	choice = bouquet;
	cout << "choice: " << choice.name << " for $";
	cout << choice.price << endl;
}


