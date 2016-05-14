#include <vector>
#include <sstream>

#include "../parser/exceptions.h"
#include "tokenstream.h"
#include "token.h"
#include "../util/util.h"

using namespace std;

namespace Tokens
{
	TokenStream::TokenStream()
	{
		this->tokens = vector<Token>();
		this->index = 0;
		this->length = 0;
	}

	void TokenStream::appendToken(Token token)
	{
		this->tokens.push_back(token);
		this->length = this->tokens.size();
	}

	string TokenStream::safePeekValue()
	{
		if (this->index < this->length)
		{
			return this->tokens.at(this->index).value;
		}
		return "";
	}

	string TokenStream::peekValue()
	{
		if (this->index < this->length)
		{
			return this->tokens.at(this->index).value;
		}
		throw new ParserException("EOF exception");
	}

	Token TokenStream::peek()
	{
		if (this->index < this->length)
		{
			return this->tokens.at(this->index);
		}
		throw new ParserException("EOF exception");
	}

	Token TokenStream::pop()
	{
		if (this->index < this->length)
		{
			return this->tokens.at(this->index++);
		}
		throw new ParserException("EOF exception");
	}

	bool TokenStream::popIfPresent(string value)
	{
		if (this->index < this->length && this->tokens.at(this->index).value == value)
		{
			this->index++;
			return true;
		}
		return false;
	}

	bool TokenStream::hasMore()
	{
		return this->index < this->length;
	}

	bool TokenStream::isNext(string value)
	{
		if (this->hasMore())
		{
			Token next = this->peek();
			if (next.value == value)
			{
				return true;
			}			
		}
		return false;
	}

	Token TokenStream::popExpected(string value)
	{
		Token output = this->pop(); // throws EOF
		if (output.value != value) 
		{
			std::stringstream msgStream;
			msgStream << "Expected: '" << value << "' but found '" << output.value << "'";
			string msg = msgStream.str();
			throw ParserException(output, msg);
		}
		return output;
	}
}
