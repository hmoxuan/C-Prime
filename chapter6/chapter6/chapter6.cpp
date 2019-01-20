// chapter6.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <climits>
#include <fstream> // for file I/O
#include <cstdlib>

// code 6.1. if -- using the if statement
void if_func(void)
{
	using namespace std;
	char ch;
	int spaces = 0;
	int total = 0;
	cin.get(ch);

	while( ch!= '.') // quit at end of sentence
	{
		if (ch == ' ') // check if ch is a space
		{
			++spaces; 
		}
		++total; // done every time
		cin.get(ch);
	}
	cout << spaces << " spaces, " << total;
	cout << " characters total in sentence" << endl;
}

// code 6.2. ifelse -- using the if else statement
void ifelse(void)
{
	using namespace std;
	char ch;
    cout << "Type, and I shall repeat." << endl;
	cin.get(ch);

	while( ch!= '.') // quit at end of sentence
	{
		if (ch == '\n') // check if ch is a space
		{
			cout << ch; 
		}
		else
		{
			cout << ++ch;
		}
		cin.get(ch);
	}
	cout << " \n Please excuse the slight confusion." << endl;
}

// code 6.3. ifelse -- using the if else if statement
const int Fave = 27;
void ifelseif(void)
{
	using namespace std;
	int n;

    cout << "Enter a number in the range 1-100 to find " << endl;
	cout << "my favoritenumber: ";
	do
	{
		cin >> n;
		if (n < Fave)
		{
			cout << "Too low -- guess again: ";
		}
	    else if (n > Fave)
		{
			cout << "Too high -- guess again: ";
		}
	    else
		{
			cout << Fave << " is right!\n";
		}
	} while (n != Fave);
}

// code 6.4. or -- using the logical OR operator
void orFunc(void)
{
	using namespace std;
	cout << "This program may reformat your hard disk \n"
		"and destory all your data.\n"
		"Do you wish to continue? <y/n> ";
	char ch;
	cin >> ch;
	if (ch == 'y' || ch =='Y')
	{
		cout << "You were warned! \a\a\n";
	}
	else if (ch == 'n' || ch =='N')
	{
		cout << "A wise choice ... bye\n";
	}
	else
	{
		cout << "That wasn't a y or n! Apparently you "
			"can't follow\n instructions, so "
			"I'll trash your disk anyway.\a\a\a\n";	
	}
}

// and.cpp -- using the logical AND operator
const int ArSize = 6;
void andFunc(void)
{
	using namespace std;
	float naaq[ArSize];
	cout << "Enter the NAAQs (New Age Awareness Quotients) " 
		<< "of\nyour neithbors. Program terminates "
		<< "when you make\n " << ArSize << " entries "
		<< "or enter a negative value."
		<< endl;

	int i = 0;
	float temp;
	cout << "First values: ";
	cin >> temp;
	while (i < ArSize && temp >= 0)
	{
		naaq[i] = temp;
		++i;
		if( i < ArSize)
		{
			cout << "Next value: ";
			cin >> temp;
		}
	}

	if (i == 0)
	{
		cout << "No data--bye\n";
	}
	else
	{
		cout << "Enter your NAAQ: ";
		float you;
		cin >> you;
		int count = 0;
		for (int j = 0; j < i; j++)
		{
			if (naaq[i] > you)
			{
				++count;
			}
		}
		cout << count;
		cout << " of your neighbors have greater awareness of \n"
		     << "the New Age than you do.\n";
	}
}

// code 6.6 more_and -- using the logical AND operator
const char * qualify[4] = // an array of pointers to strings
{
	"10.000-meter race.\n",
	"mud tug-of-war.\n",
	"masters canoe jousting.\n",
	"pie-hrowing festival.\n",
};
void more_add(void)
{
	using namespace std;
	int age;
	cout << "Enter your age in years: ";
	cin >> age;
	int index;

	if (age > 17 && age < 35)
	{
		index = 0;
	}
	else if (age > 35 && age < 50)
	{
		index = 2;
	}
	else
	{
		index = 3;
	}
	cout << "You qualify for the " << qualify[index];
}

// code 6.7 notFunc -- using the not operator
bool is_int(double x)
{
	if (x <= INT_MAX && x >= INT_MIN) // use climits values
	{
		return true;
	}
	else
	{
		return false;
	}
}

void notFunc( void )
{
	using namespace std;
	double num;

	cout << "Yo, dude! Enter an interger value: ";
	cin >> num;
	while (!is_int(num))
	{
		cout << "Out of range -- please try again: ";
		cin >> num;
	}
	int val = int (num);
	cout << "You've entered the integer " << val << "\nBye\n";
}

// code 6.8 cctypes -- using the ctype.h library
void cctypes(void) // prototypes for character functions
{
	using namespace std;
	cout << "Enter text for analysis, and type @"
		" to terminate input.\n";
	char ch;
	int whitespace = 0;
	int digits = 0;
	int chars = 0;
	int punct = 0;
	int others = 0;

	cin.get(ch); // get first character
	while( ch != '@')
	{
		if(isalpha(ch))
			chars++;
		else if(isspace(ch))
			whitespace++;
		else if(isdigit(ch))
			digits++;
		else if(ispunct(ch))
			punct++;
		else
			others++;
		cin.get(ch); // get next character
	}
	cout << chars << " letters, " 
		<< whitespace << " whiteaspace, "
		<< digits << " digits, "
		<< punct << " punctuations, "
		<< others << " others.\n";
}

// code 6.9 conditFunc -- using the conditional operator
void conditFunc(void) // prototypes for character functions
{
	using namespace std;
	int a, b;
	cout << "Enter two integers: ";
	cin >> a >> b;
	cout << "The larger of " << a << " and " << b;
	int c = a > b ? a : b;
	cout << "is " << c << endl;
}

// code 6.10 swithcFunc -- using the switch statement
void showmenu(void)
{
    using namespace std;

	cout << "Please enter 1, 2, 3, 4, or 5: \n"
		"1) alarm		2) report\n"
		"3) alibi		4) comfort\n"
		"5) quit \n";
}
void report(void)
{
    using namespace std;

	cout << "It's been an excellent week for business. \n"
		"Sales are up 120%. Expenses are down 35S%. \n";
}
void comfort(void)
{
    using namespace std;

	cout << "Your employees think you are the finest CEO \n"
		"in the industry. The board of directors think\n";
		"you are the finest CEO in the industry. \n";
}
void swithcFunc(void) // prototypes for character functions
{
	using namespace std;
	showmenu();
    int choice;
	cin >> choice;
	while (choice != 5)
	{
		switch (choice)
		{
			case 1:
				cout << "\a\n";
				break;
			case 2:
				report();
				break;
			case 3:
				cout << "The boss was in all day.\n";
				break;
			case 4:
				comfort();
				break;		
			default:
				cout << "That's not a choice.\n";		
		}
		showmenu();
		cin >> choice;
	}
	cout << "Bye!\n";
}

// code 6.11 emumFunc -- using enum
enum {red, orange, yellow, green, blue, violet, indigo};
void enumFunc(void)
{
    using namespace std;
	cout << "Enter color code (0-6): ";
	int code;
	cin >> code;
	while (code >= red && code <= indigo)
	{
		switch (code)
		{
			case red: cout << "Her lips wer red.\n"; break;
			case orange: cout << "Her lips wer orange.\n"; break;
			case yellow: cout << "Her lips wer yellow.\n"; break;
			case green: cout << "Her lips wer green.\n"; break;
			case blue: cout << "Her lips wer blue.\n"; break;
			case violet: cout << "Her lips wer violet.\n"; break;
			case indigo: cout << "Her lips wer indigo.\n"; break;
		}
		cout << "Enter color code (0-6): ";
		cin >> code;
	}
	cout << "Bye\n";
}

// code 6.12 jumpFunc -- using continue and break
void jumpFunc(void)
{
    using namespace std;
	const int ArSize = 80;
	char line[ArSize];
	int spaces = 0;

	cout << "Enter a line of text: \n";
	cin.get(line, ArSize);
	cout << "Complete line:\n" << line << endl;
	cout << "Line through first period:\n";

	for (int i = 0; line[i] != '\0'; i++)
	{
		cout << line[i];
		if (line[i] == '.')
			break;
		if (line[i] != ' ')
			continue;
		spaces++;
	}
	cout << "\n" << spaces << " spaces\n";
	cout << "Done.\n";
}

// code 6.13 cinfish -- non-numeric input terminates loop
void cinfish(void)
{
    using namespace std;
	const int Max = 5;
	double fish[Max];
	cout << "Please enter the weights of your finsh.\n";
	cout << "You may enter up to " << Max
		<< " fish <q to terminate> \n";
	cout << "fish #1: ";
	int i = 0;
	while (i < Max && cin >> fish[i])
	{
		if (++i < Max)
		{
			cout << "fish #" << i+1 << ": ";
		}
	}

	// calculate average
	double total = 0.0;
	for (int j = 0; j < i; j++)
	{
		total += fish[j];
	}

	// report results
	if (i == 0)
	{
		cout << "No fish\n";
	}
	else
	{
		cout << total / i << " = average weight of"
			<< i << " fish\n";
	}
	cout << "Done.\n";
}

// code 6.14 cingolf -- non-numeric input skipped
void cingolf(void)
{
    using namespace std;
	const int Max = 5;
	// get data
	int golf[Max];
	cout << "Please enter your golf scores.\n";
	cout << "You must enter " << Max << " rounds.\n";
	int i;
	for (i = 0; i < Max; i++)
	{
		cout << "round #" << i+1 << ": ";
		while ( !(cin >> golf[i]) )
		{
			cin.clear(); // reset input
			while (cin.get() != '\n')
				continue;
			cout << "Please enter a number: ";
		}
	}

	// calculate average
	double total = 0.0;
	for (i = 0; i < Max; i++)
	{
		total += golf[i];
	}

	// report results
    cout << total / Max << " = average score " << Max << " rounds\n";
}

// code 6.15 outfile -- writing to a file
void outfile(void)
{
    using namespace std;
	char automobile[50];
	int year;
	double a_price;
	double d_price;

	ofstream outFile; // create object for output
	outFile.open("carinfo.txt"); // associate with a file
	cout << "Enter the make and model of automobil: ";
	cin.getline(automobile, 50);
	cout << "Enter the model  year: ";
	cin >> year;
	cout << "Enter the original asking price: ";
	cin >> a_price;
	d_price = 0.913 * a_price;

	// display information on screen with cout
	cout << fixed;
	cout.precision(2);
	cout.setf(ios_base::showpoint);
	cout << "Make and model: " << automobile <<endl;
	cout << "Year:" << year << endl;
	cout << "Was asking $" << a_price << endl;
	cout << "Now asking $" << d_price << endl;

	// now do exact same things using outFile instead of cout
	outFile << fixed;
	outFile.precision(2);
	outFile.setf(ios_base::showpoint);
	outFile << "Make and model: " << automobile << endl;
	outFile << "Year: " << year << endl;
	cout << "Was asking $" << a_price << endl;
	cout << "Now asking $" << d_price << endl;

	outFile.close(); // done with file
}

// code 6.16 sumafile -- functions with an array argument
void sumafile(void)
{
    using namespace std;
	const int SIZE = 60;
	char filename[SIZE];
	ifstream inFile; // object for handling file input
	cout << "Ener name of data file: ";
	cin.getline(filename, SIZE);
	inFile.open(filename);
	if (!inFile.is_open())
	{
		cout << "Could not open the file " << filename << endl;
		cout << "Program terminating.\n";
		exit(EXIT_FAILURE);
	}

	double value;
	double sum = 0.0; // number of items read
	int count = 0;
	inFile >> value;
	while (inFile.good())
	{
		++count; // one more item read
		sum += value;
		inFile >> value; // get next value
	}
	
	if (inFile.eof())
	{
		cout << "End of file reached.\n";
	}
	else if (inFile.fail())
	{
		cout << "Input terminated by daa mismach.\n";
	}
	else
	{
		cout << "Input terminated for unknown reason.\n";
	}

    if (count == 0)
	{
		cout << "No data processed.\n";
	}
	else
	{
		cout << "Items read: " << count << endl;
		cout << "Sum: " << sum << endl;
		cout << "Average: " << sum / count << endl;
	}
	inFile.close();
}

int _tmain(int argc, _TCHAR* argv[])
{
	using namespace std;
	cout << "chapter6" << endl;
	//if_func();
	//ifelse();
	//ifelseif();
	//orFunc();
	//andFunc();
	//more_add();
	//notFunc();
	//cctypes();
	//conditFunc();
	//swithcFunc();
	//enumFunc();
	//jumpFunc();
	//cinfish();
	//cingolf();
	//outfile();
	sumafile();


	system("pause");
	return 0;
}
