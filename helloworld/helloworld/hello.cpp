#include "stdafx.h"
#include <iostream>
using namespace std;

int getinfo(void)
{
	int carrots;
	cout << "--------getinfo--------" <<endl;
	cout << "How many carrots do you have?" << endl;
	cin >> carrots;
	cout << "Here are two more.";
	carrots = carrots +2;

	//the next line concatenates output
	cout << "Now you have " << carrots << " carrots." << endl;
	return 0;
}

int _tmain(int  argc, _TCHAR* argv[])
{
	int carrots;
	carrots = 25;

	cout << "I have ";
	cout << carrots;
	cout << " carrots. ";
	cout << endl;

	carrots = carrots -1;
	cout << "Crunch, crunch. Now I have" << carrots << " carrots." << endl;

	getinfo();

	system("pause");
	return 0;
}