#include <iostream>
#include <vector>

#include "exceptions.h"
#include "expressionparser.h"
#include "filecontext.h"
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
	Expression* parseAddition(TokenStream* tokens, FileContext* fileContext);
	Expression* parseEntity(TokenStream* tokens, FileContext* fileContext);
	Expression* parseEntitySuffixes(TokenStream* tokens, FileContext* fileContext);
	Expression* parseInequality(TokenStream* tokens, FileContext* fileContext);
	Expression* parseParenthesis(TokenStream* tokens, FileContext* fileContext);
	Expression* parseTernary(TokenStream* tokens, FileContext* fileContext);

	// This function doesn't do anything special, but it's easier to call this instead
	// of remembering that parseTernary is the outermost of the order of operations.
	Expression* parseExpression(TokenStream* tokens, FileContext* fileContext)
	{
		return parseTernary(tokens, fileContext);
	}

	Expression* parseTernary(TokenStream* tokens, FileContext* fileContext)
	{
		Expression* expression = parseInequality(tokens, fileContext);
		if (tokens->safePeekValue() == "?")
		{
			Token questionToken = tokens->pop();
			Expression* trueExpression = parseTernary(tokens, fileContext);
			tokens->popExpected(":");
			Expression* falseExpression = parseTernary(tokens, fileContext);
			expression = new Ternary(expression, questionToken, trueExpression, falseExpression, fileContext);
		}
		return expression;
	}

	Expression* parseInequality(TokenStream* tokens, FileContext* fileContext)
	{
		Expression* expression = parseAddition(tokens, fileContext);
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
				expressions.push_back(parseAddition(tokens, fileContext));
				next = tokens->safePeekValue();
			}
			return new BinaryOpChain(expressions, ops, fileContext);
		}
		return expression;
	}

	Expression* parseAddition(TokenStream* tokens, FileContext* fileContext)
	{
		Expression* expression = parseEntitySuffixes(tokens, fileContext);
		string next = tokens->safePeekValue();
		if (next == "-" || next == "+")
		{
			vector<Token> ops = vector<Token>();
			vector<Expression*> expressions = vector<Expression*>();
			expressions.push_back(expression);
			while (next == "-" || next == "+")
			{
				ops.push_back(tokens->pop());
				expressions.push_back(parseEntitySuffixes(tokens, fileContext));
				next = tokens->safePeekValue();
			}
			return new BinaryOpChain(expressions, ops, fileContext);
		}
		return expression;
	}

	Expression* parseEntitySuffixes(TokenStream* tokens, FileContext* fileContext)
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

		Expression* expression = parseParenthesis(tokens, fileContext);
		next = tokens->safePeekValue();
		while (next == "." || next == "[" || next == "(")
		{
			if (next == ".")
			{
				Token dotToken = tokens->pop();
				Token fieldNameToken = tokens->pop();
				fieldNameToken.assertValidIdentifier();
				return new DotField(expression, dotToken, fieldNameToken, fileContext);
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
						Expression* index = parseExpression(tokens, fileContext);
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
					expression = new BracketIndex(expression, openBracket, indices.at(0), fileContext);
				}
				else if (indices.size() == 2)
				{
					expression = new BracketSlice(
						expression, openBracket, indices.at(0), indices.at(1), NULL, fileContext);
				}
				else
				{
					expression = new BracketSlice(
						expression, openBracket, indices.at(0), indices.at(1), indices.at(2), fileContext);
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
					Expression* arg = parseExpression(tokens, fileContext);
					args.push_back(arg);
				}
				expression = new FunctionInvocation(expression, openParen, args, fileContext);
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
			expression = new InlineIncrement(prefixToken, expression, prefixToken, true, fileContext);
		}

		if (hasSuffix)
		{
			expression = new InlineIncrement(
				expression->firstToken, expression, suffixToken, false, fileContext);
		}

		return expression;
	}

	Expression* parseParenthesis(TokenStream* tokens, FileContext* fileContext)
	{
		if (tokens->safePeekValue() == "(")
		{
			Token openParen = tokens->pop();
			Expression* expression = parseExpression(tokens, fileContext);
			tokens->popExpected(")");
			return expression;
		}
		return parseEntity(tokens, fileContext);
	}

	Expression* parseEntity(TokenStream* tokens, FileContext* fileContext)
	{
		if (!tokens->hasMore())
		{
			// TODO: pass in TokenStream instance and at least say which file.
			throw new ParserException("Unexpected EOF encountered.");
		}

		string next = tokens->peekValue();
		if (next == "null") return new NullConstant(tokens->pop(), fileContext);
		if (next == "true") return new BooleanConstant(tokens->pop(), true, fileContext);
		if (next == "false") return new BooleanConstant(tokens->pop(), false, fileContext);

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
				Expression* key = parseExpression(tokens, fileContext);
				tokens->popExpected(":");
				Expression* value = parseExpression(tokens, fileContext);
				nextItemProhibited = !tokens->popIfPresent(",");
				dictionaryKeys.push_back(key);
				dictionaryValues.push_back(value);
			}
			return new Dictionary(dictionaryToken, dictionaryKeys, dictionaryValues, fileContext);
		}

		if (c >= '0' && c <= '9')
		{
			// TODO: recognize floats in the tokenizer and then search for the period here.
			int intValue;
			if (string2Integer(next, &intValue))
			{
				return new IntegerConstant(tokens->pop(), intValue, fileContext);
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
			return new StringConstant(tokens->pop(), actualValue, fileContext);
		}

		if ((c >= 'a' && c <= 'z') ||
			(c >= 'A' && c <= 'Z') ||
			c == '_')
			// TODO: '$' should only be a valid character in the translator context.
		{
			return new Variable(tokens->pop(), next, fileContext);
		}

		throw new ParserException(tokens->pop(),
			"parseEntity encountered something weird: '" + next + "'");
	}
}
