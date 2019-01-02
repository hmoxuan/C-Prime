// strtype3.c : Code 4.9 -- assigning, adding, add appending
//
#include "stdafx.h"
#include <iostream>
#include <string>  //make string class available
#include <cstring>  // C-style string library

#include "strtype3.h"

void strtype3(void)
{
	using namespace std;
	char charr1[20];  // create an empty array
	char charr2[20] = "jaguar";  // create an initialized array
	string str1;  // create an empty string object
	string str2 = "Panther";  // create an initialized string

	// assignment for string objects and character arrays
	str1 = str2;  //copy str2 to str1
	strcpy_s(charr1, charr2);  // copy charr2 to charr1

	// appending for striing objects and character arrays
	str1 += " paste";  // add paste to end of str1
	strcat_s(charr1, " juice");  // add juice to end of charr1

	// finding the length of a string object and a C-Style string
	int len1 = str1.size();  // obtain length of str1
	int len2 = strlen(charr1);  // obtain length of charr1

	cout << "The string " << str1 << " contains "
		<< len1 << " characters.\n";
	cout << "The string " << charr1 << " contains "
		<< len2 << " characters.\n";
}


