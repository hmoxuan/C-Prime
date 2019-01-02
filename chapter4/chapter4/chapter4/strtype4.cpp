// strtype4.h : Code 4.10 -- line input
//
#include "stdafx.h"
#include <iostream>
#include <string>  //make string class available
#include <cstring>  // C-style string library

#include "strtype4.h"

void strtype4(void)
{
	using namespace std;
	char charr[20];  // create an empty array
	string str;  // create an empty string object

	cout << "Length of string in charr before input: "
		<< strlen(charr) << endl;
	cout << "Length of string in str before input: "
		<< str.size() << endl;

	cout << "Enter a line of text: \n";
	cin.getline(charr, 20);  // indicate maximum length
	cout << "You entered: " << charr << endl;

	cout << "Enter another line of text: \n";
	getline(cin, str);  // cin now an argument; indicate maximum length
	cout << "You entered: " << charr << endl;

	cout << "Length of string in charr after input: "
		<< strlen(charr) << endl;
	cout << "Length of string in str after input: "
		<< str.size() << endl;
}


