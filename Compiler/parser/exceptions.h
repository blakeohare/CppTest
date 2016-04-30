#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <iostream>
#include <sstream>
#include <string>

#include "../tokens/token.h"

using namespace Tokens;

class ParserException
{
public:
	ParserException(string message)
	{
		this->message = message;
		// TODO: catch this centrally and display the message instead of these silly cout's.
		cout << this->message << endl;
	}

	ParserException(Token token, string message)
	{
		stringstream ss1;
		ss1 << (token.line + 1);
		stringstream ss2;
		ss2 << (token.col + 1);
		this->message = "";
		this->message += token.file;
		this->message += " Line: ";
		this->message += ss1.str();
		this->message += " Column: ";
		this->message += ss2.str();
		this->message += " --> ";
		this->message += message;

		cout << this->message << endl;
	}

	string message;
};

#endif
