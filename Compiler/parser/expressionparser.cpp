#include <vector>
#include <iostream>

#include "exceptions.h"
#include "expressionparser.h"
#include "nodes.h"
#include "../tokens/token.h"
#include "../tokens/tokenstream.h"
#include "../util/util.h"

using namespace std;
using namespace Tokens;
using namespace Nodes;
using namespace Parser;

namespace Parser
{
	Expression* parseAddition(TokenStream* tokens);
	Expression* parseEntity(TokenStream* tokens);
	Expression* parseEntitySuffixes(TokenStream* tokens);
	Expression* parseInequality(TokenStream* tokens);
	Expression* parseParenthesis(TokenStream* tokens);
	Expression* parseTernary(TokenStream* tokens);

	Expression* parseExpression(TokenStream* tokens)
	{
		Expression* output = parseTernary(tokens);
		return output;
	}

	Expression* parseTernary(TokenStream* tokens)
	{
		Expression* expression = parseInequality(tokens);
		if (tokens->safePeekValue() == "?")
		{
			Token questionToken = tokens->pop();
			Expression* trueExpression = parseTernary(tokens);
			tokens->popExpected(":");
			Expression* falseExpression = parseTernary(tokens);
			expression = new Ternary(expression, questionToken, trueExpression, falseExpression);
		}
		return expression;
	}

	Expression* parseInequality(TokenStream* tokens)
	{
		Expression* expression = parseAddition(tokens);
		string next = tokens->safePeekValue();
		if (next == "<" || next == ">" || next == "<=" || next == ">=")
		{
			// TODO: inequality chaining: "a < b < c"
			vector<Token> ops = vector<Token>();
			vector<Expression*> expressions = vector<Expression*>();
			expressions.push_back(expression);
			while (next == "<" || next == ">" || next == "<=" || next == ">=")
			{
				ops.push_back(tokens->pop());
				expressions.push_back(parseAddition(tokens));
				next = tokens->safePeekValue();
			}
			return new BinaryOpChain(expressions, ops);
		}
		return expression;
	}

	Expression* parseAddition(TokenStream* tokens)
	{
		Expression* expression = parseEntitySuffixes(tokens);
		string next = tokens->safePeekValue();
		if (next == "-" || next == "+")
		{
			vector<Token> ops = vector<Token>();
			vector<Expression*> expressions = vector<Expression*>();
			expressions.push_back(expression);
			while (next == "-" || next == "+")
			{
				ops.push_back(tokens->pop());
				expressions.push_back(parseEntitySuffixes(tokens));
				next = tokens->safePeekValue();
			}
			return new BinaryOpChain(expressions, ops);
		}
		return expression;
	}

	Expression* parseEntitySuffixes(TokenStream* tokens)
	{
		bool hasPrefix = false;
		bool hasSuffix = false;
		Token prefixToken;
		Token suffixToken;
		string next = tokens->safePeekValue();
		if (next == "++" || next == "--")
		{
			prefixToken = tokens->pop();
			hasPrefix = true;
		}

		Expression* expression = parseParenthesis(tokens);
		next = tokens->safePeekValue();
		while (next == "." || next == "[" || next == "(")
		{
			if (next == ".")
			{
				Token dotToken = tokens->pop();
				Token fieldNameToken = tokens->pop();
				fieldNameToken.assertValidIdentifier();
				return new DotField(expression, dotToken, fieldNameToken);
			}
			else if (next == "[")
			{
				Token openBracket = tokens->pop();
				vector<Expression*> indices = vector<Expression*>();
				bool nextAllowed = true;
				while (!tokens->popIfPresent("]"))
				{
					if (tokens->popIfPresent(":"))
					{
						nextAllowed = true;
						indices.push_back(NULL);
					}
					else
					{
						Expression* index = parseExpression(tokens);
						nextAllowed = tokens->popIfPresent(":");
						indices.push_back(index);
					}
				}

				if (nextAllowed)
				{
					indices.push_back(NULL);
				}

				if (indices.size() > 3)
				{
					throw new ParserException(openBracket, 
						"Too many expressions in this slice expression.");
				}

				if (indices.size() == 1)
				{
					if (indices.at(0) == NULL)
					{
						throw new ParserException(openBracket, "Need an index expression here.");
					}
					expression = new BracketIndex(expression, openBracket, indices.at(0));
				}
				else if (indices.size() == 2)
				{
					expression = new BracketSlice(
						expression, openBracket, indices.at(0), indices.at(1), NULL);
				}
				else
				{
					expression = new BracketSlice(
						expression, openBracket, indices.at(0), indices.at(1), indices.at(2));
				}
			}
			else if (next == "(")
			{
				vector<Expression*> args = vector<Expression*>();
				Token openParen = tokens->popExpected("(");
				while (!tokens->popIfPresent(")"))
				{
					if (args.size() > 0)
					{
						tokens->popExpected(",");
					}
					Expression* arg = parseExpression(tokens);
					args.push_back(arg);
				}
				expression = new FunctionInvocation(expression, openParen, args);
				next = tokens->safePeekValue();
			}
			next = tokens->safePeekValue();
		}

		if (next == "++" || next == "--")
		{
			suffixToken = tokens->pop();
			hasSuffix = true;
		}

		if (hasPrefix && hasSuffix)
		{
			throw new ParserException(suffixToken, "Expression has both suffix and prefix");
		}

		if (hasPrefix)
		{
			expression = new InlineIncrement(prefixToken, expression, prefixToken, true);
		}

		if (hasSuffix)
		{
			expression = new InlineIncrement(
				expression->firstToken, expression, suffixToken, false);
		}

		return expression;
	}

	Expression* parseParenthesis(TokenStream* tokens)
	{
		if (tokens->safePeekValue() == "(")
		{
			Token openParen = tokens->pop();
			Expression* expression = parseExpression(tokens);
			tokens->popExpected(")");
			return expression;
		}
		return parseEntity(tokens);
	}

	Expression* parseEntity(TokenStream* tokens)
	{
		if (!tokens->hasMore())
		{
			// TODO: pass in TokenStream instance and at least say which file.
			throw new ParserException("Unexpected EOF encountered.");
		}

		string next = tokens->peekValue();
		if (next == "null") return new NullConstant(tokens->pop());
		if (next == "true") return new BooleanConstant(tokens->pop(), true);
		if (next == "false") return new BooleanConstant(tokens->pop(), false);

		char c = next[0];

		if (c == '{')
		{
			Token dictionaryToken = tokens->popExpected("{");
			vector<Expression*> dictionaryKeys = vector<Expression*>();
			vector<Expression*> dictionaryValues = vector<Expression*>();
			bool nextItemProhibited = false;
			while (!tokens->popIfPresent("}"))
			{
				if (nextItemProhibited)
				{
					tokens->popExpected(","); // throw a reasonable exception.
				}
				Expression* key = parseExpression(tokens);
				tokens->popExpected(":");
				Expression* value = parseExpression(tokens);
				nextItemProhibited = !tokens->popIfPresent(",");
				dictionaryKeys.push_back(key);
				dictionaryValues.push_back(value);
			}
			return new Dictionary(dictionaryToken, dictionaryKeys, dictionaryValues);
		}

		if (c >= '0' && c <= '9')
		{
			// TODO: recognize floats in the tokenizer and then search for the period here.
			int intValue;
			if (string2Integer(next, &intValue))
			{
				return new IntegerConstant(tokens->pop(), intValue);
			}
			throw new ParserException(tokens->pop(), "Invalid integer value.");
		}

		if (c == '"' || c == '\'')
		{
			string actualValue = "";
			for (int i = 1; i < next.size() - 1; ++i)
			{
				c = next[i];
				if (c == '\\')
				{
					if (i == next.size() - 2)
					{
						throw new ParserException(tokens->pop(), 
							"String cannot end in a backslash.");
					}

					c = next[++i];
					switch (c)
					{
						case 'n': actualValue += "\n"; break;
						case 'r': actualValue += "\r"; break;
						case '"': actualValue += "\""; break;
						case '\'': actualValue += "'"; break;
						case '0': actualValue += "\0"; break;
						case 't': actualValue += "\t"; break;
						case '\\': actualValue += "\\"; break;
						default:
							throw new ParserException(tokens->pop(), 
								"String contains an unknown escape sequence.");
					}
				}
				else
				{
					actualValue += c;
				}
			}
			return new StringConstant(tokens->pop(), actualValue);
		}

		if ((c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			c == '_')
			// TODO: '$' should only be a valid character in the translator context.
		{
			return new Variable(tokens->pop(), next);
		}

		throw new ParserException(tokens->pop(),
			"parseEntity encountered something weird: '" + next + "'");
	}
}
