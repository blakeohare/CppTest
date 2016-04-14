#include <vector>
#include <sstream>

#include "tokenstream.h"
#include "token.h"
#include "../util/util.h"

using namespace std;

namespace Tokens
{
	TokenStream::TokenStream(vector<Token*>* tokens)
	{
		this->tokens = tokens;
		this->index = 0;
		this->length = tokens->size();
	}

	Token* TokenStream::peek()
	{
		if (this->index < this->length)
		{
			return this->tokens->at(this->index);
		}
		throw "EOF exception";
	}

	Token* TokenStream::pop()
	{
		if (this->index < this->length)
		{
			return this->tokens->at(this->index++);
		}
		throw "EOF exception";
	}

	bool TokenStream::hasMore()
	{
		return this->index < this->length;
	}

	bool TokenStream::isNext(string value)
	{
		if (this->hasMore())
		{
			Token* next = this->peek();
			if (streq(next->value, value))
			{
				return true;
			}			
		}
		return false;
	}

	Token* TokenStream::popExpected(string value)
	{
		Token* output = this->pop(); // throws EOF
		if (!streq(output->value, value)) 
		{
			std::stringstream msgStream;
			msgStream << "Expected: '" << value << "' but found '" << output->value << "'";
			string msg = msgStream.str();
			throw msg;
		}
		return output;
	}
}
