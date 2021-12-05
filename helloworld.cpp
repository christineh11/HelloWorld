#include <iostream>
#include "std_lib_facilities.h"

/*
	***Always end your input in terminal with semi colon";" (EX: 4-3;)
	In order to quit program type "quit"

	Exercise 1: Under scores are allowed when assigning variable names
	to use "Let", use "L" followed by a variable name (EX: L x_1=4;)
	x_1=4

	Exercise 2: After assigning variable with let, to change it use "="
	(EX: = x_1=2;)
	Now, x_1=2

	Exercise 3: To define constants use "const" (EX: const pi=3.14;)


*/

/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 Christine Hawkins
 */

struct Token
{
	char kind;
	double value;
	string name;
	Token(char ch) : kind(ch), value(0) {}
	Token(char ch, double val) : kind(ch), value(val) {}
	Token(char ch, string val) : kind(ch), name(val) {} // Error 1: Line missing
};

class Token_stream
{
	bool full;
	Token buffer;

public:
	Token_stream() : full(0), buffer(0) {}
	Token get();
	void unget(Token t)
	{
		buffer = t;
		full = true;
	}
	void ignore(char);
};

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';
// exercise 2: It can be useful to change the assignment of a variable after using let to
//  test different values into that variable or if you inputted the wrong value for that variable.
// However, it can be a source of problems if you wanted that variable value to stay constant
// and it was changed using this assignment operator
const char change = '=';
const char constant = 'C';

Token Token_stream::get()
{
	if (full)
	{
		full = false;
		return buffer;
	}
	char ch;
	cin >> ch;
	switch (ch)
	{
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
	{
		return Token(ch);
	}

	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{
		cin.unget();
		double val;
		cin >> val;
		return Token(number, val);
	}
	default:
	{
		// exercise 1
		if (isalpha(ch) || ch == '_')
		{
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_'))
			{
				s += ch; // Error 2
			}
			cin.unget();
			if (s == "quit")
				return Token(quit);

			// exercise 2
			if (s == "L")
				return Token(let);
			if (s == "=")
				return Token(change);

			// exercise 3
			if (s == "const")
				return Token(constant);

			return Token(name, s);
		}
		error("Bad token");
		return Token(' ');
	}
	}
}

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind)
	{
		full = false;
		return;
	}
	full = false;
	char ch;
	while (cin >> ch)
	{
		if (ch == c)
			return;
	}
}

struct Variable
{
	string name;
	double value;

	Variable(string n, double v) : name(n), value(v) {}
};

vector<Variable> names;

double get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)
	{
		if (names[i].name == s)
		{
			return names[i].value;
		}
	}
	error("get: undefined name ", s);
	return 0.0; // return statment
}

void set_value(string s, double d)
{
	for (int i = 0; i <= names.size(); ++i)
	{
		if (names[i].name == s)
		{
			names[i].value = d;
			return;
		}
	}
	error("set: undefined name ", s);
}

bool is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
	{
		if (names[i].name == s)
			return true;
	}
	return false;
}

Token_stream ts;

double expression();

double primary()
{
	Token t = ts.get();
	switch (t.kind)
	{
	case '(':
	{
		double d = expression();
		t = ts.get();
		if (t.kind != ')')
			error("')' expected");
		return d; // return statment
	}
	case '-':
	{
		return -primary();
	}
	case '+':
	{
		return primary();
	}
	case number:
	{
		return t.value;
	}
	case name:
	{
		return get_value(t.name);
	}
	default:
	{
		error("primary expected");
		return 0.0; // return statment
	}
	}
}

double term()
{
	double left = primary();
	while (true)
	{
		Token t = ts.get();
		switch (t.kind)
		{

		case '*':
		{
			left *= primary();
			break;
		}
		case '/':
		{
			double d = primary();
			if (d == 0)
				error("divide by zero");
			left /= d;
			break;
		}
		default:
		{
			ts.unget(t);
			return left;
		}
		}
	}
}

double expression()
{
	double left = term();
	while (true)
	{
		Token t = ts.get();
		switch (t.kind)
		{
		case '+':
		{
			left += term();
			break;
		}
		case '-':
		{
			left -= term();
			break;
		}
		default:
		{
			ts.unget(t);
			return left;
		}
		}
	}
}

double declaration()
{
	Token t = ts.get();
	if (t.kind != name)
	{
		error("name expected in declaration");
	}

	string name = t.name;
	Token t2 = ts.get();

	if (t2.kind != '=')
		error("= missing in declaration of ", name);
	double d = expression();
	if (is_declared(name))
		set_value(name, d);

	names.push_back(Variable(name, d));
	return d;
}

double statement()
{
	Token t = ts.get();
	double d;
	switch (t.kind)
	{
	case let:
		return declaration();
	case change:
		return declaration();
	case constant:
		return declaration();
	default:
		ts.unget(t);
		d = expression();
	}



	return d;
}

void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";

void calculate()
{
	while (true)
		try
		{
			cout << prompt;
			Token t = ts.get();
			while (t.kind == print)
			{
				t = ts.get();
			}
			if (t.kind == quit)
			{
				return;
			}
			ts.unget(t);
			cout << result << statement() << endl;
		}
		catch (runtime_error &e)
		{
			cerr << e.what() << endl;
			clean_up_mess();
		}
}

int main()

try
{
	calculate();
	return 0;
}
catch (exception &e)
{
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';')
		;
	return 1;
}
catch (...)
{
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';')
		;
	return 2;
}