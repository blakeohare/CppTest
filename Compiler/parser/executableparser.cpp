
#include <iostream>
#include <string>
#include <vector>

#include "exceptions.h"
#include "executableparser.h"
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
	Executable* parseClass(TokenStream* tokens);
	Executable* parseConstructor(TokenStream* tokens);
	Executable* parseField(TokenStream* tokens);
	Executable* parseForLoop(TokenStream* tokens);
	Executable* parseFunction(TokenStream* tokens);
	Executable* parseIf(TokenStream* tokens);

	Executable* parseClass(TokenStream* tokens)
	{
		vector<Executable*> instanceFields = vector<Executable*>();
		vector<Executable*> staticFields = vector<Executable*>();
		vector<Executable*> instanceFunctions = vector<Executable*>();
		vector<Executable*> staticFunctions = vector<Executable*>();
		Executable* staticConstructor = NULL;
		Executable* instanceConstructor = NULL;
		vector<Token> baseClasses = vector<Token>();

		Token classToken = tokens->popExpected("class");
		Token nameToken = tokens->pop();
		// TODO: verify name token is valid identifier.
		if (tokens->popIfPresent(":"))
		{
			while (!tokens->popIfPresent("{"))
			{
				if (baseClasses.size() > 0)
				{
					tokens->popExpected(",");
				}
				Token baseClassToken = tokens->pop();
				// TODO: verify baseClassToken is valid identifier.

			}
		}
		else
		{
			tokens->popExpected("{");
		}

		while (!tokens->popIfPresent("}"))
		{
			bool isStatic = tokens->safePeekValue() == "static";
			Token staticToken;
			if (isStatic) staticToken = tokens->pop();

			string next = tokens->safePeekValue();
			if (next == "function")
			{
				FunctionDefinition* functionDefinition =
					(FunctionDefinition*) parseFunction(tokens);
				if (isStatic)
				{
					functionDefinition->isStatic = true;
					functionDefinition->firstToken = staticToken;
				}
			}
			else if (next == "field")
			{
				FieldDefinition* field = (FieldDefinition*) parseField(tokens);
				if (isStatic)
				{
					field->isStatic = true;
					field->firstToken = staticToken;
				}
			}
			else if (next == "constructor")
			{
				if (isStatic && staticConstructor != NULL)
				{
					throw new ParserException(staticToken, 
						"Classes cannot have multiple static constructors.");
				}
				if (!isStatic && instanceConstructor != NULL)
				{
					throw new ParserException(tokens->peek(),
						"Classes cannot have multiple constructors.");
				}
				ConstructorDefinition* constructor =
					(ConstructorDefinition*) parseConstructor(tokens);
			}
			else
			{
				throw new ParserException(tokens->pop(), "Unexpected token: '" + next + "'");
			}
		}

		throw new ParserException("Class parsing not implemented.");
	}

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

	Executable* parseConstructor(TokenStream* tokens)
	{
		Token constructorToken = tokens->popExpected("constructor");
		tokens->popExpected("(");
		vector<Token> argNames = vector<Token>();
		vector<Expression*> argValues = vector<Expression*>();
		vector<Expression*> baseArgs = vector<Expression*>();
		while (!tokens->popIfPresent(")"))
		{
			if (argNames.size() > 0)
			{
				tokens->popExpected(",");
			}
			Token argName = tokens->pop();
			argName.assertValidIdentifier();
			argNames.push_back(argName);
			Expression* argValue = NULL;
			if (tokens->popIfPresent("="))
			{
				argValue = parseExpression(tokens);
			}
			argValues.push_back(argValue);
		}

		if (tokens->popIfPresent(":"))
		{
			tokens->popExpected("base");
			tokens->popExpected("(");
			while (!tokens->popIfPresent(")"))
			{
				if (baseArgs.size() > 0)
				{
					tokens->popExpected(",");
				}
				baseArgs.push_back(parseExpression(tokens));
			}
		}

		vector<Executable*> code = parseCodeBlock(tokens, true);

		return new ConstructorDefinition(constructorToken, argNames, argValues, baseArgs, code);
	}

	Executable* parseExecutable(TokenStream* tokens, bool allowComplex, bool semicolonExpected)
	{
		string next = tokens->peekValue();
		if (allowComplex)
		{
			if (next == "class") return parseClass(tokens);
			if (next == "for") return parseForLoop(tokens);
			if (next == "function") return parseFunction(tokens);
			if (next == "if") return parseIf(tokens);

			if (next == "return")
			{
				throw new ParserException(tokens->pop(), "Return not implemented yet.");
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
			throw new ParserException(tokens->pop(),
				"This sort of expression isn't allowed here.");
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

	Executable* parseField(TokenStream* tokens)
	{
		Token fieldToken = tokens->popExpected("field");
		Token nameToken = tokens->pop();
		nameToken.assertValidIdentifier();
		Expression* defaultValue = NULL;
		if (tokens->popIfPresent("="))
		{
			defaultValue = parseExpression(tokens);
		}
		tokens->popExpected(";");
		return new FieldDefinition(fieldToken, nameToken, defaultValue);
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
			nameToken,
			argNames,
			argValues,
			body);

		return functionDefinition;
	}

	Executable* parseIf(TokenStream* tokens)
	{
		Token ifToken = tokens->popExpected("if");
		tokens->popExpected("(");
		Expression* condition = parseExpression(tokens);
		tokens->popExpected(")");
		vector<Executable*> trueCode = parseCodeBlock(tokens, false);
		vector<Executable*> falseCode;
		bool hasFalseCode = tokens->popIfPresent("else");
		if (hasFalseCode)
		{
			falseCode = parseCodeBlock(tokens, false);
		}
		return new IfStatement(ifToken, condition, trueCode, hasFalseCode, falseCode);
	}

	vector<Executable*>* parseInterpretedCode(string rootFolder)
	{
		return NULL;
	}
}
