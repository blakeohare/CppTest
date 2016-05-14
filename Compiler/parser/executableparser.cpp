
#include <iostream>
#include <string>
#include <vector>

#include <sstream>

#include "exceptions.h"
#include "executableparser.h"
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
	Executable* parseClass(TokenStream* tokens, FileContext* fileContext);
	Executable* parseConstructor(TokenStream* tokens, FileContext* fileContext);
	Executable* parseField(TokenStream* tokens, FileContext* fileContext);
	Executable* parseForLoop(TokenStream* tokens, FileContext* fileContext);
	Executable* parseFunction(TokenStream* tokens, FileContext* fileContext);
	Executable* parseIf(TokenStream* tokens, FileContext* fileContext);
	Executable* parseImport(TokenStream* tokens, FileContext* fileContext);
	Executable* parseNamespace(TokenStream* tokens, FileContext* fileContext);
	Executable* parseReturn(TokenStream* tokens, FileContext* fileContext);

	Executable* parseClass(TokenStream* tokens, FileContext* fileContext)
	{
		vector<Executable*> instanceFields = vector<Executable*>();
		vector<Executable*> staticFields = vector<Executable*>();
		vector<Executable*> instanceMethods = vector<Executable*>();
		vector<Executable*> staticMethods = vector<Executable*>();
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
					(FunctionDefinition*) parseFunction(tokens, fileContext);
				if (isStatic)
				{
					functionDefinition->isStatic = true;
					functionDefinition->firstToken = staticToken;
					staticMethods.push_back(functionDefinition);
				}
				else
				{
					instanceMethods.push_back(functionDefinition);
				}
			}
			else if (next == "field")
			{
				FieldDefinition* field = (FieldDefinition*) parseField(tokens, fileContext);
				if (isStatic)
				{
					field->isStatic = true;
					field->firstToken = staticToken;
					staticFields.push_back(field);
				}
				else
				{
					instanceFields.push_back(field);
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
					(ConstructorDefinition*) parseConstructor(tokens, fileContext);
				if (isStatic)
				{
					if (constructor->baseArgs.size() > 0)
						throw new ParserException(constructor->firstToken,
							"Static constructor cannot invoke a base constructor.");
					staticConstructor = constructor;
				}
				else
				{
					instanceConstructor = constructor;
				}
			}
			else
			{
				throw new ParserException(tokens->pop(), "Unexpected token: '" + next + "'");
			}
		}

		return new ClassDefinition(
			classToken,
			nameToken,
			baseClasses,
			staticConstructor,
			staticFields,
			staticMethods,
			instanceConstructor,
			instanceFields,
			instanceMethods,
			fileContext);
	}

	vector<Executable*> parseCodeBlock(TokenStream* tokens, FileContext* fileContext, bool bracketsRequired)
	{
		vector<Executable*> output = vector<Executable*>();
		bool bracketsPresent = bracketsRequired || (tokens->safePeekValue() == "{");
		if (bracketsPresent)
		{
			tokens->popExpected("{");
			while (!tokens->popIfPresent("}"))
			{
				output.push_back(parseExecutable(tokens, fileContext, true, true));
			}
		}
		else
		{
			output.push_back(parseExecutable(tokens, fileContext, true, true));
		}
		return output;
	}

	Executable* parseConstructor(TokenStream* tokens, FileContext* fileContext)
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
				argValue = parseExpression(tokens, fileContext);
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
				baseArgs.push_back(parseExpression(tokens, fileContext));
			}
		}

		vector<Executable*> code = parseCodeBlock(tokens, fileContext, true);

		return new ConstructorDefinition(constructorToken, argNames, argValues, baseArgs, code, fileContext);
	}

	Executable* parseExecutable(
		TokenStream* tokens, 
		FileContext* fileContext,
		bool allowComplex,
		bool semicolonExpected)
	{
		string next = tokens->peekValue();
		if (allowComplex)
		{
			if (next == "import") return parseImport(tokens, fileContext);
			if (next == "class") return parseClass(tokens, fileContext);
			if (next == "for") return parseForLoop(tokens, fileContext);
			if (next == "function") return parseFunction(tokens, fileContext);
			if (next == "if") return parseIf(tokens, fileContext);
			if (next == "namespace") return parseNamespace(tokens, fileContext);
			if (next == "return") return parseReturn(tokens, fileContext);
		}

		Expression* expression = parseExpression(tokens, fileContext);
		if (tokens->hasMore())
		{
			next = tokens->peekValue();
			if (next[next.size() - 1] == '=')
			{
				stringstream s;
				s << tokens->peek().line;

				// It's an assignment
				Token assignmentToken = tokens->pop();
				Expression* assignmentValue = parseExpression(tokens, fileContext);
				if (semicolonExpected)
				{
					tokens->popExpected(";");
				}
				return new Assignment(expression, assignmentToken, assignmentValue, fileContext);
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
		return new ExpressionAsExecutable(expression, fileContext);
	}

	void parseExecutables(TokenStream* tokens, vector<Executable*>* target, FileContext* fileContext)
	{
		while (tokens->hasMore())
		{
			Executable* executable = parseExecutable(tokens, fileContext, true, true);
			target->push_back(executable);
		}
	}

	Executable* parseField(TokenStream* tokens, FileContext* fileContext)
	{
		Token fieldToken = tokens->popExpected("field");
		Token nameToken = tokens->pop();
		nameToken.assertValidIdentifier();
		Expression* defaultValue = NULL;
		if (tokens->popIfPresent("="))
		{
			defaultValue = parseExpression(tokens, fileContext);
		}
		tokens->popExpected(";");
		return new FieldDefinition(fieldToken, nameToken, defaultValue, fileContext);
	}

	Executable* parseForLoop(TokenStream* tokens, FileContext* fileContext)
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
				init.push_back(parseExecutable(tokens, fileContext, false, false));
			}
		}

		if (!tokens->popIfPresent(";"))
		{
			condition = parseExpression(tokens, fileContext);
			tokens->popExpected(";");
		}

		while (!tokens->popIfPresent(")"))
		{
			if (step.size() > 0)
			{
				tokens->popExpected(",");
			}
			step.push_back(parseExecutable(tokens, fileContext, false, false));
		}

		vector<Executable*> code = parseCodeBlock(tokens, fileContext, false);

		return new ForLoop(forToken, init, condition, step, code, fileContext);
	}

	Executable* parseFunction(TokenStream* tokens, FileContext* fileContext)
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
				argValue = parseExpression(tokens, fileContext);
			}
			argNames.push_back(argName);
			argValues.push_back(argValue);
		}
		vector<Executable*> body = parseCodeBlock(tokens, fileContext, true);

		return new FunctionDefinition(
			functionToken,
			functionToken,
			nameToken,
			argNames,
			argValues,
			body,
			fileContext);
	}

	Executable* parseIf(TokenStream* tokens, FileContext* fileContext)
	{
		Token ifToken = tokens->popExpected("if");
		tokens->popExpected("(");
		Expression* condition = parseExpression(tokens, fileContext);
		tokens->popExpected(")");
		vector<Executable*> trueCode = parseCodeBlock(tokens, fileContext, false);
		vector<Executable*> falseCode;
		bool hasFalseCode = tokens->popIfPresent("else");
		if (hasFalseCode)
		{
			falseCode = parseCodeBlock(tokens, fileContext, false);
		}
		return new IfStatement(ifToken, condition, trueCode, hasFalseCode, falseCode, fileContext);
	}

	Executable* parseImport(TokenStream* tokens, FileContext* fileContext)
	{
		Token importToken = tokens->popExpected("import");
		vector<string> names = vector<string>();
		Token nameToken = tokens->pop();
		nameToken.assertValidIdentifier();
		names.push_back(nameToken.value);
		while (tokens->popIfPresent("."))
		{
			nameToken = tokens->pop();
			nameToken.assertValidIdentifier();
			names.push_back(nameToken.value);
		}
		tokens->popExpected(";");

		// Add ImportStatement to fileContext only in the top level to ensure that it
		// is imported correctly at the root, at the top of the file, rather than adding
		// it here. Note that in translated code, imports can occur anywhere in the file
		// and mean something different.
		return new ImportStatement(importToken, names, fileContext);
	}

	Executable* parseNamespace(TokenStream* tokens, FileContext* fileContext)
	{
		Token namespaceToken = tokens->popExpected("namespace");
		Token nameToken = tokens->pop();
		nameToken.assertValidIdentifier();
		vector<Executable*> members = vector<Executable*>();
		tokens->popExpected("{");
		while (!tokens->popIfPresent("}"))
		{
			Executable* member = parseExecutable(tokens, fileContext, true, true);
			members.push_back(member);
		}
		return new Namespace(namespaceToken, nameToken, members, fileContext);
	}

	Executable* parseReturn(TokenStream* tokens, FileContext* fileContext)
	{
		throw new ParserException(tokens->pop(), "Parse return isn't implemented yet.");
	}
}
