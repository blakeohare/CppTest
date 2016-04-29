#include <string>

#include "token.h"
#include "../parser/parser.h"

using namespace std;

namespace Tokens
{
	void Token::assertValidIdentifier()
	{
		bool valid = true;
		string value = this->value;
		char c = value[0];
		if (c >= '0' && c <= '9')
		{
			valid = false;
		}
		else
		{
			for (int i = value.size() - 1; i >= 0; --i)
			{
				c = value[i];
				if ((c < '0' || c > '9') &&
					(c < 'a' || c > 'z') &&
					(c < 'A' || c > 'Z') &&
					c != '_')
				{
					valid = false;
					break;
				}
			}
		}

		// TODO: lookup table of reserved keywords.
		if (!valid)
		{
			throw new Parser::ParserException(*this, "Valid identifier expected.");
		}
	}
}
