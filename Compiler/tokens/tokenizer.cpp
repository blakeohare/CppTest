#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "tokenstream.h"
#include "token.h"

using namespace std;

namespace Tokens
{
	enum TokenizerMode
	{
		NORMAL,
		STRING,
		COMMENT,
		WORD,
	};

	TokenStream* tokenize(string* filename, string contents)
	{
		vector<Token*>* tokens = new vector<Token*>();

		int length = contents.length();
		int lines[length];
		int cols[length];
		int line = 0;
		int col = 0;
		char c;
		int i;
		for (i = 0; i < length; ++i)
		{
			lines[i] = line;
			cols[i] = col;
			c = contents[i];
			if (c == '\n')
			{
				col = 0;
				line++;
			}
			else
			{
				col++;
			}
		}

		TokenizerMode mode = NORMAL;
		char modeType = '\0';
		char c2;
		string tokenBuilder = "";
		int tokenStart = 0;
		int x = 0;
		for (i = 0; i < length; ++i)
		{
			c = contents[i];
			c2 = i + 1 < length ? contents[i + 1] : '\0';

			switch (mode)
			{
				case NORMAL:
					if (c == ' ' || c == '\t' || c == '\n' || c == '\r')
					{
						// skip whitespace.
					}
					else if (c == '/' && c2 == '/')
					{
						modeType = '/';
						mode = COMMENT;
					}
					else if (c == '/' && c2 == '*')
					{
						++i;
						modeType = '*';
						mode = COMMENT;
					}
					else if (c == '_' ||
						c == '$' ||
						(c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c >= '0' && c <= '9'))
					{
						tokenStart = i;
						tokenBuilder = "";
						mode = WORD;
						--i;
					}
					else if (c == '"' || c == '\'')
					{
						mode = STRING;
						modeType = c;
						tokenStart = i;
						tokenBuilder = "";
						tokenBuilder += c;
					}
					else
					{
						tokenBuilder = "";
						tokenBuilder += c;
						int tokenSize = 1;
						switch (c)
						{
							case '+': if (c2 == '+' || c2 == '=') tokenSize = 2; break;
							case '-': if (c2 == '-' || c2 == '=') tokenSize = 2; break;
							case '<': if (c2 == '<' || c2 == '=') tokenSize = 2; break; // TODO: tokenSize = 3 for <<=
							case '>': if (c2 == '>' || c2 == '=') tokenSize = 2; break; // TODO: tokenSize = 3 for >>=
							case '*': if (c2 == '*' || c2 == '=') tokenSize = 2; break; // TODO: tokenSize = 3 for **=
							case '=': if (c2 == '=') tokenSize = 2; break;
							case '!': if (c2 == '=') tokenSize = 2; break;
							case '&': if (c2 == '&') tokenSize = 2; break;
							case '|': if (c2 == '|') tokenSize = 2; break;
							case '?': if (c2 == '?') tokenSize = 2; break;
							case '%': if (c2 == '=') tokenSize = 2; break;
							case '/': if (c2 == '=') tokenSize = 2; break;
							case '^': if (c2 == '=') tokenSize = 2; break;
							default: break;
						}

						tokenStart = i;
						if (tokenSize == 2) {
							tokenBuilder += c2;
							++i;
						}

						tokens->push_back(new Token(lines[tokenStart], cols[tokenStart], tokenBuilder, filename));
					}
					break;

				case STRING:
					if (c == modeType)
					{
						tokenBuilder += c;
						tokens->push_back(new Token(lines[tokenStart], cols[tokenStart], tokenBuilder, filename));
						tokenBuilder = "";
						mode = NORMAL;
					}
					else
					{
						if (c == '\\' && i + 1 < length)
						{
							tokenBuilder += '\\';
							tokenBuilder += c2;
							++i;
						}
						tokenBuilder += c;
					}
					break;

				case COMMENT:
					if (modeType == '/')
					{
						if (c == '\n')
						{
							mode = NORMAL;
						}
					}
					else // modeType == '*'
					{
						if (c == '*' && c2 == '/')
						{
							mode = NORMAL;
							++i;
						}
					}
					break;

				case WORD:
					if (c == '_' || c == '$' ||
						(c >= 'a' && c <= 'z') ||
						(c >= 'A' && c <= 'Z') ||
						(c >= '0' && c <= '9'))
					{
						tokenBuilder += c;
					}
					else
					{
						tokens->push_back(new Token(lines[tokenStart], cols[tokenStart], tokenBuilder, filename));
						--i;
						tokenBuilder = "";
						mode = NORMAL;
					}
					break;
			}
		}
		return new TokenStream(tokens);
	}
}
