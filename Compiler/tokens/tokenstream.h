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
			vector<Token*>* tokens;

			Token* peek();
			Token* pop();
			bool hasMore();
			bool isNext(string value);
			Token* popExpected(string value);

			TokenStream(vector<Token*>* tokens);
	};
}
#endif
