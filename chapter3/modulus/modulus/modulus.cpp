// modulus.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <iostream>

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	const int Lbs_per_stn = 14;
	int lbs;

	cout << "Enter your weight in pounds: ";
	cin >> lbs;

	int stone = lbs / Lbs_per_stn;// whole stone
	int pounds = lbs % Lbs_per_stn;// remainder in pounds
	cout << lbs << " Pounds are " << stone
		<< " stone," << pounds << "Pound(s)" <<endl;
	
	system("pause");
	return 0;
}
