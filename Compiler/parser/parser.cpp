#include <vector>
#include <iostream>
#include "parser.h"
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
	Executable* parseForLoop(TokenStream* tokens);
	Executable* parseFunction(TokenStream* tokens);
	Expression* parseInequality(TokenStream* tokens);
	Expression* parseParenthesis(TokenStream* tokens);
	Expression* parseTernary(TokenStream* tokens);

	vector<Executable*> parseCodeBlock(TokenStream* tokens, bool bracketsRequired)
	{
		vector<Executable*> output = vector<Executable*>();
		bool bracketsPresent = bracketsRequired || (tokens->safePeekValue() == "{");
		if (bracketsPresent)
		{
			tokens->popExpected("{");
			while (!tokens->popIfPresent("}"))
			{
				output.push_back(parseExecutable(tokens, true, true));
			}
		}
		else
		{
			output.push_back(parseExecutable(tokens, true, true));
		}
		return output;
	}

	Executable* parseFunction(TokenStream* tokens)
	{
		Token functionToken = tokens->popExpected("function");
		Token nameToken = tokens->pop();
		// TODO: verify valid identifier
		tokens->popExpected("(");
		vector<Token> argNames = vector<Token>();
		vector<Expression*> argValues = vector<Expression*>();
		while (!tokens->popIfPresent(")"))
		{
			if (argNames.size() > 0)
			{
				tokens->popExpected(",");
			}
			Token argName = tokens->pop();
			Expression* argValue = NULL;
			// TODO: verify valid identifier
			if (tokens->popIfPresent("="))
			{
				argValue = parseExpression(tokens);
			}
			argNames.push_back(argName);
			argValues.push_back(argValue);
		}
		vector<Executable*> body = parseCodeBlock(tokens, true);

		FunctionDefinition* functionDefinition = new FunctionDefinition(
			functionToken,
			functionToken,
			false, // is static
			nameToken,
			argNames,
			argValues,
			body);

		cout << "Function returned!" << endl;

		return functionDefinition;
	}

	vector<Executable*>* parseInterpretedCode(string rootFolder)
	{
		return NULL;
	}

	Executable* parseExecutable(TokenStream* tokens, bool allowComplex, bool semicolonExpected)
	{
		string next = tokens->peekValue();
		if (allowComplex)
		{
			if (next == "function")
			{
				return parseFunction(tokens);
			}

			if (next == "for")
			{
				return parseForLoop(tokens);
			}

			if (next == "return")
			{
				cout << "Return not implemented yet." << endl;
				throw "Not implemented.";
			}
		}

		Expression* expression = parseExpression(tokens);
		if (tokens->hasMore())
		{
			next = tokens->peekValue();
			if (next[next.size() - 1] == '=')
			{
				// It's an assignment
				Token assignmentToken = tokens->pop();
				Expression* assignmentValue = parseExpression(tokens);
				if (semicolonExpected)
				{
					tokens->popExpected(";");
				}
				return new Assignment(expression, assignmentToken, assignmentValue);
			}
		}

		if (expression->type != FUNCTION_INVOCATION &&
			expression->type != INLINE_INCREMENT)
		{
			cout << "This sort of expression isn't allowed here." << endl;
			throw ":(";
		}
		if (semicolonExpected)
		{
			tokens->popExpected(";");
		}
		return new ExpressionAsExecutable(expression);
	}

	void parseExecutables(TokenStream* tokens, vector<Executable*>* target)
	{
		while (tokens->hasMore())
		{
			Executable* executable = parseExecutable(tokens, true, true);
			target->push_back(executable);
		}
	}

	Executable* parseForLoop(TokenStream* tokens)
	{
		Token forToken = tokens->pop();
		tokens->popExpected("(");
		vector<Executable*> init = vector<Executable*>();
		vector<Executable*> step = vector<Executable*>();
		Expression* condition = NULL;
		if (!tokens->popIfPresent(";"))
		{
			while (!tokens->popIfPresent(";"))
			{
				if (init.size() > 0)
				{
					tokens->popExpected(",");
				}
				init.push_back(parseExecutable(tokens, false, false));
			}
		}

		if (!tokens->popIfPresent(";"))
		{
			condition = parseExpression(tokens);
			tokens->popExpected(";");
		}

		while (!tokens->popIfPresent(")"))
		{
			if (step.size() > 0)
			{
				tokens->popExpected(",");
			}
			step.push_back(parseExecutable(tokens, false, false));
		}

		vector<Executable*> code = parseCodeBlock(tokens, false);

		return new ForLoop(forToken, init, condition, step, code);
	}

	Expression* parseExpression(TokenStream* tokens)
	{
		return parseTernary(tokens);
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
				cout << "dot step not implemented" << endl;
				throw "Not implemented";
			}
			else if (next == "[")
			{
				cout << "bracket not implemented" << endl;
				throw "Not implemented";
			}
			else if (next == "(")
			{
				cout << "Found a function invocation" << endl;
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
		}

		next = tokens->safePeekValue();
		if (next == "++" || next == "--")
		{
			suffixToken = tokens->pop();
			hasSuffix = true;
		}

		if (hasPrefix && hasSuffix)
		{
			cout << "Expression has both suffix and prefix" << endl;
			throw ":(";
		}

		if (hasPrefix)
		{
			expression = new InlineIncrement(prefixToken, expression, prefixToken, true);
		}

		if (hasSuffix)
		{
			expression = new InlineIncrement(expression->firstToken, expression, suffixToken, false);
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
			cout << "parseEntity encountered EOF" << endl;
			throw ":(";
		}

		string next = tokens->peekValue();
		if (next == "null") return new NullConstant(tokens->pop());
		if (next == "true") return new BooleanConstant(tokens->pop(), true);
		if (next == "false") return new BooleanConstant(tokens->pop(), false);

		char c = next[0];
		if (c >= '0' && c <= '9')
		{
			// TODO: recognize floats in the tokenizer and then search for the period here.
			int intValue;
			if (string2Integer(next, &intValue))
			{
				return new IntegerConstant(tokens->pop(), intValue);
			}
			cout << "invalid integer" << endl;
			throw "Invalid integer";
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
						cout << "String cannot end in a backslash." << endl;
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
							cout << "Unknown escape sequence: \\" << c << endl;
							throw "Unknown escape sequence";
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
			cout << "Yes, I found the variable." << endl;
			return new Variable(tokens->pop(), next);
		}

		cout << "parseEntity encountered something weird: '" << next << "'" << endl;
		throw "Unknown thingy: '" + next + "'";
	}
}
