#ifndef TOKENSTREAM_H
#define TOKENSTREAM_H

#include <vector>

#include "token.h"

using namespace std;

namespace Tokens
{
	class TokenStream
	{
		public:
			int index;
			int length;
			vector<Token> tokens;

			void appendToken(Token token);
			Token peek();
			string peekValue();
			string safePeekValue();
			Token pop();
			bool hasMore();
			bool isNext(string value);
			Token popExpected(string value);
			bool popIfPresent(string value);

			TokenStream();
	};
}
#endif
