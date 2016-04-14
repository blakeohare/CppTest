#ifndef TOKEN_H
#define TOKEN_H

#include <string>

using namespace std;

namespace Tokens
{
	class Token
	{
		public:
			int line;
			int col;
			string value;
			string file;

			Token(int line, int col, string value, string file)
			{
				this->line = line;
				this->col = col;
				this->value = value;
				this->file = file;
			}
	};
}

#endif
