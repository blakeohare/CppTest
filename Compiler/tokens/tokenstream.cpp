#include <vector>

#include "tokenstream.h"
#include "token.h"

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
}
