// limits.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>
#include <climits>

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	int n_int = INT_MAX;
	short n_short = SHRT_MAX;
	long n_long = LONG_MAX;
	long long n_llong = LLONG_MAX;

	// sizeof operator yields size of type or of variable
	cout << "int is " << sizeof (int) << " bytes." << endl;
	cout << "short is " << sizeof n_short << " bytes." << endl;
	cout << "long is " << sizeof n_long << " bytes." << endl;
	cout << "long long is " << sizeof n_llong << " bytes." << endl;
	cout << endl;
		 
   cout << "Maximum values: " << endl;
   cout << "int: " << n_int << endl;
   cout << "shourt: " << n_short << endl;
   cout << "long: " << n_long << endl;
   cout << "long long: " << n_llong << endl << endl;

   cout << "Minimum int value = " << INT_MIN << endl;
   cout << "Bits per byte = " << CHAR_BIT << endl;

   //---------------------------------------------
   // C++11 Initialization variable
   //---------------------------------------------
   int hamburgers = { 24 };  //set hamburgers to 24
//   int emus{7};//set emus to 7
   int rheas = { 12 };  //set rheas to 12
//   int rocs = {};  // set rocs to 0
//   int psychics{};  //set psychis to 0

   cout << " hamburgers = " << hamburgers << endl;
//   cout << " emus = " << emus;
   cout << " rheas = " << rheas << endl;
//   cout << " rocs = " << rocs << endl;
//   cout << " psyschics = " << psychics;


    system("pause");
	return 0;
}

