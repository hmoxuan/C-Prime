// arrayone.cpp : Code 4.1 small arrays of integers
//
#include "stdafx.h"
#include <iostream>

#include "arrayone.h"

void arrayone(void)
{
	using namespace std;
	int yams[3]; // creates array with three elements
	yams[0] = 7;  // assign value to first element
	yams[1] = 8;
	yams[2] = 6;


// NOTE: If your C++ compiler or translator can't initialize
// this array, use static int yamcosts[3] instead of
// int yamcosts[3]
	int yamcosts[3] = {20, 30, 5};  // create, initialize array

	cout << "Total yams = ";
    cout << yams[0] +  yams[1] +  yams[2] << endl;
	cout << "The package with " << yams[1] << " yams costs ";
	cout << yamcosts[1] << " cents per yam.\n";

	int total = yams[0] * yamcosts[0] + yams[1] * yamcosts[1];
	total =  total  + yams[2] * yamcosts[2];
	cout <<"The total yam expense is " << total << " cents.\n";

	cout << "\nSize of yams array = " << sizeof yams << "bytes.\n";
	cout << "\nSize of one element = " << sizeof yams[0] << "bytes.\n";

	float hotelTips[5] = {5.0, 2.5};
	short things[] = {1, 5, 3, 8};
	int num_elements =  sizeof things /sizeof (short);

	cout << "hotelTips[0] = " << hotelTips[0] << endl;
	cout << "hotelTips[1] = " << hotelTips[1] << endl;
	cout << "hotelTips[2] = " << hotelTips[2] << endl;
	cout << "hotelTips[3] = " << hotelTips[3] << endl;
	cout << "hotelTips[4] = " << hotelTips[4] << endl;
    cout << "num_elements of short things[] = " << num_elements <<endl;
}