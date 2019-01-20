// chapter6.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <iostream>
#include <climits>

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
	jumpFunc();


	system("pause");
	return 0;
}
