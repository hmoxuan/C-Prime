// pointer.h : pointer 
//
#include "stdafx.h"
#include <iostream>
#include <cstring>  // declare strlen(), strcpy()
#include "pointer.h"

#pragma warning(disable:4996)  //if you want use strcpy function

// pointerFun: Code 4.15 our first pointer variable
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

// code 4.16 -- initialize a pointer
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

// code 4.17 -- using the new pointer
void use_new(void)
{
	using namespace std;
	int nights = 1001;
	int * pt = new int;  // allocate space for an int
	*pt = 1001;

	cout << "nights value = ";
	cout << nights << ": location = " << &nights << endl;
	cout << "int ";
	cout << "value = " << *pt << ": location = " << pt << endl;

	double * pd = new double;  // allocate space for an double
	*pd = 10000001.0;  // store a double there
	cout << "double ";
	cout << "value = " << *pd << ": location = " << pd << endl;
	cout << "location of pointer pd: " <<  &pd << endl;

	cout << "size of pt = " << sizeof(pt);
	cout << " : size of *pt = " << sizeof(*pt) << endl;
	cout << "size of pd = " << sizeof(pd);
	cout << " : size of *pd = " << sizeof(*pd) << endl;
}


// code 4.18 -- arraynew: using the new operator for arrayss
void arraynew(void)
{
	using namespace std;
	double * p3 = new double [3]; // space for 3 doubles
	p3[0] = 0.2;
	p3[1] = 0.5;
	p3[2] = 0.8;
	cout << "p3[1] is " << p3[1] << ".\n";
	p3 = p3 + 1;
	cout << "Now p3[0] is " << p3[0] << " and ";
	cout << "p3[1] is " << p3[1] << ".\n";
	p3 = p3 - 1;
	delete [] p3; // free the memory
}

// code 4.19 -- pointer addition
void addpntrs(void)
{
	using namespace std;
	double wages[3] = {10000.0, 20000.0, 30000.0};
	short stacks[3] = {3, 2, 1};

	// Here are two ways to get the address of array
	double * pw = wages; // name of anarray = address
	short * ps = &stacks[0]; // or use address operator

	// with array element
	cout << "pw = " << pw << ", pw = " << *pw << endl;
	pw = pw + 1;
	cout << "add 1 to the pw pointer:\n";
	cout << "pw = " << pw << ", pw = " << *pw << "\n\n";

	cout << "ps = " << ps << ", ps = " << *ps << endl;
	ps = ps + 1;
	cout << "add 1 to the ps pointer:\n";
	cout << "ps = " << ps << ", ps = " << *ps << "\n\n";

	cout << "access two elements with array notation\n";
	cout << "stack[0] = " << stacks[0] << ", stack[1] = " << stacks[1] << endl; 

	cout << "access two elements with pointer notation\n";
	cout << "*stacks = " << *stacks << ", *(stacks + 1) = " << *(stacks + 1) << endl; 

	cout << sizeof(wages) << " = size of wages array\n";
	cout << sizeof(pw) << " = size of pw pointer\n";
}

// code 4.20 -- using pointers to stringss
void ptrstr(void)
{
	using namespace std;
	char animal[20] = "bear"; // animal holds bear
	const char * bird = "wren"; // bird holds address of string
	char * ps; // uninitialized
	cout << animal << " and " << bird << endl;
	// cout << ps << endl; // may display garbage, may cause a crash

	cout << "Enter a kind of animal: ";
	cin >> animal; // ok if input < 20 chars
	//cin >> ps; // Too horrible a blunder to try, ps doesn't point to allocated space

	ps = animal; // set ps to point to string
	cout << ps << "!\n";
	cout << "Before using strcpy():\n";
	cout << animal << " at " << (int *) animal << endl;
	cout << ps << " at " << (int *) ps << endl;

	ps = new char[strlen(animal) + 1]; // get new storage
	strcpy(ps, animal);
	cout << "After using strcpy():\n";
	cout << animal << " at " << (int *) animal << endl;
	cout << ps << " at " << (int *) ps << endl;

	delete [] ps;
}