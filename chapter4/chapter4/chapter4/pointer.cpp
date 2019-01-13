// pointer.h : Code 4.15 our first pointer variable
//
#include "stdafx.h"
#include <iostream>

#include "pointer.h"

void pointerFun(void)
{
	using namespace std;
	int updates = 6;  // declare a variable
	int * p_updates;  // declare pointer to an int 
	p_updates = &updates;  // assign address of int to pointer

	// express values two ways
	cout << "Values: updates = " << updates;
	cout << ", *p_updates = " << *p_updates << endl;

    // express address two ways
	cout << "Addresses: &updates = " << &updates;
	cout << ", p_updates = " << p_updates << endl;

	// use pointer to change value
	*p_updates = *p_updates + 1;
	cout <<"Now update =" << updates << endl;
}

void int_ptr(void)
{
	using namespace std;
	int higgens = 5;  // declare a variable
	int * pt = &higgens;  // declare pointer to an int 

	// express values two ways
	cout << "Values of higgens = " << higgens;
	cout << ", Addresses of higgens = "  << &higgens << endl;

	// express values two ways
	cout << "Values of *pt = " << *pt;
	cout << ", Addresses of pt = "  << pt << endl;
}

