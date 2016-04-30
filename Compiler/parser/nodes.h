#ifndef NODES_H
#define NODES_H

#include <vector>

#include "../tokens/token.h"

using namespace std;
using namespace Tokens;

namespace Nodes
{
	enum NodeType
	{
		UNKNOWN_NODE,

		ASSIGNMENT,
		BINARY_OP_CHAIN,
		BOOLEAN_CONSTANT,
		CLASS_DEFINITION,
		CONSTRUCTOR_DEFINITION,
		DICTIONARY,
		EXPRESSION_AS_EXECUTABLE,
		FIELD_DEFINITION,
		FOR_LOOP,
		FUNCTION_DEFINITION,
		FUNCTION_INVOCATION,
		IF_STATEMENT,
		INLINE_INCREMENT,
		INTEGER_CONSTANT,
		NULL_CONSTANT,
		STRING_CONSTANT,
		TERNARY,
		VARIABLE,
	};

	enum BinaryOpType
	{
		UNKNOWN_BINARY_OP,

		ADDITION,
		SUBTRACTION,
		MULTIPLICATION,
		DIVISION,
		MODULO,
		EQUALS,
		NOT_EQUALS,
	};

	class ParseNode;
	class Executable;
	class Expression;

	class ParseNode
	{
	public:

		ParseNode(NodeType type, Token firstToken) {
			this->type = type;
			this->firstToken = firstToken;
		}
		NodeType type;
		Token firstToken;
		Executable* owner;

		virtual void SetLocalIdPass() { };
	};

	class Executable : public ParseNode
	{
	public:
		Executable(NodeType type, Token firstToken)
			: ParseNode(type, firstToken)
		{ }

		virtual void SetLocalIdPass() { };
	};

	class Expression : public ParseNode
	{
	public:
		Expression(NodeType type, Token firstToken)
			: ParseNode(type, firstToken)
		{ }

		virtual void SetLocalIdPass() { };
	};

	////////////////////////////////////////////////////////
	// Everything is alphabetical now... ///////////////////
	////////////////////////////////////////////////////////

	class Assignment : public Executable
	{
	public:
		//Assignment() : Executable() { }
		Assignment(
			Expression* targetExpression,
			Token assignmentToken,
			Expression* valueExpression)
			: Executable(ASSIGNMENT, targetExpression->firstToken)
		{
			this->targetExpression = targetExpression;
			this->assignmentToken = assignmentToken;
			this->valueExpression = valueExpression;
		}
		Expression* targetExpression;
		Expression* valueExpression;
		Token assignmentToken;
		// TODO: enum for op

		virtual void SetLocalIdPass();
	};

	class BinaryOpChain : public Expression
	{
	public:
		BinaryOpChain(vector<Expression*> expressions, vector<Token> ops)
			: Expression(BINARY_OP_CHAIN, expressions.at(0)->firstToken)
		{
			this->expressions = expressions;
			this->ops = ops;
		}
		vector<Expression*> expressions;
		vector<Token> ops;

		virtual void SetLocalIdPass();
	};

	class BooleanConstant : public Expression
	{
	public:
		BooleanConstant(Token token, bool value)
			: Expression(BOOLEAN_CONSTANT, token)
		{
			this->value = value;
		}

		bool value;

		virtual void SetLocalIdPass();
	};

	class ConstructorDefinition : public Executable
	{
	public:
		ConstructorDefinition(
			Token firstToken,
			vector<Token> argNames,
			vector<Expression*> argValues,
			vector<Expression*> baseArgs,
			vector<Executable*> code)
			: Executable(CONSTRUCTOR_DEFINITION, firstToken)
		{
			this->isStatic = false;
			this->argNames = argNames;
			this->argValues = argValues;
			this->baseArgs = baseArgs;
			this->code = code;
		}

		bool isStatic;
		vector<Token> argNames;
		vector<Expression*> argValues;
		vector<Expression*> baseArgs;
		vector<Executable*> code;

		virtual void SetLocalIdPass();
	};

	class Dictionary : public Expression
	{
	public:
		Dictionary(
			Token firstToken,
			vector<Expression*> keys,
			vector<Expression*> values)
			: Expression(DICTIONARY, firstToken)
		{
			this->keys = keys;
			this->values = values;
		}

		vector<Expression*> keys;
		vector<Expression*> values;

		virtual void SetLocalIdPass();
	};

	class ExpressionAsExecutable : public Executable
	{
	public:
		ExpressionAsExecutable(Expression* expression)
			: Executable(EXPRESSION_AS_EXECUTABLE, expression->firstToken)
		{
			this->expression = expression;
		}
		Expression* expression;
		virtual void SetLocalIdPass();
	};

	class FieldDefinition : public Executable
	{
	public:
		FieldDefinition(Token firstToken, Token nameToken, Expression* value)
			: Executable(FIELD_DEFINITION, firstToken)
		{
			this->isStatic = false;
			this->nameToken = nameToken;
			this->defaultValue = value;
		}
		bool isStatic;
		Token nameToken;
		Expression* defaultValue;

		virtual void SetLocalIdPass();
	};

	class ForLoop : public Executable
	{
	public:
		ForLoop(
			Token forToken,
			vector<Executable*> init,
			Expression* condition,
			vector<Executable*> step,
			vector<Executable*> code)
			: Executable(FOR_LOOP, forToken)
		{
			this->init = init;
			this->condition = condition;
			this->step = step;
			this->code = code;
		}

		vector<Executable*> init;
		Expression* condition;
		vector<Executable*> step;
		vector<Executable*> code;

		virtual void SetLocalIdPass();
	};

	class FunctionDefinition : public Executable
	{
	public:
		FunctionDefinition(
			Token firstToken,
			Token functionToken,
			Token nameToken,
			vector<Token> argNames,
			vector<Expression*> argValues,
			vector<Executable*> body)
			: Executable(FUNCTION_DEFINITION, firstToken)
		{
			this->functionToken = functionToken;
			this->isStatic = false;
			this->nameToken = nameToken;
			this->name = nameToken.value;
			this->argNames = argNames;
			this->argValues = argValues;
			this->body = body;
		}
		Token functionToken;
		bool isStatic;
		Token nameToken;
		string name;
		vector<Token> argNames;
		vector<Expression*> argValues;
		vector<Executable*> body;

		virtual void SetLocalIdPass();
	};

	class FunctionInvocation : public Expression
	{
	public:
		FunctionInvocation(Expression* root, Token openParen, vector<Expression*> args)
			: Expression(FUNCTION_INVOCATION, root->firstToken)
		{
			this->root = root;
			this->args = args;
			this->openParen = openParen;
		}
		Expression* root;
		Token openParen;
		vector<Expression*> args;

		virtual void SetLocalIdPass();
	};

	class IfStatement : public Executable
	{
	public:
		IfStatement(
			Token ifToken,
			Expression* condition,
			vector<Executable*> trueCode,
			bool hasFalseCode,
			vector<Executable*> falseCode)
			: Executable(IF_STATEMENT, ifToken)
		{
			this->condition = condition;
			this->trueCode = trueCode;
			this->falseCode = falseCode;
		}
		Expression* condition;
		vector<Executable*> trueCode;
		bool hasFalseCode;
		vector<Executable*> falseCode;
		
		virtual void SetLocalIdPass();
	};

	class InlineIncrement : public Expression
	{
	public:
		InlineIncrement(
			Token firstToken,
			Expression* expression,
			Token incrementToken,
			bool isPrefix) 
			: Expression(INLINE_INCREMENT, firstToken)
		{
			this->expression = expression;
			this->incrementToken = incrementToken;
			this->isPrefix = isPrefix;
		}
		Expression* expression;
		Token incrementToken;
		bool isPrefix;

		virtual void SetLocalIdPass();
	};

	class IntegerConstant : public Expression
	{
	public:
		IntegerConstant(Token token, int value) : Expression(INTEGER_CONSTANT, token)
		{
			this->value = value;
		}
		int value;

		virtual void SetLocalIdPass();
	};

	class NullConstant : public Expression
	{
	public:
		NullConstant(Token token) : Expression(NULL_CONSTANT, token) { }

		virtual void SetLocalIdPass();
	};

	class StringConstant : public Expression
	{
	public:
		StringConstant(Token token, string value) : Expression(STRING_CONSTANT, token)
		{
			this->value = value;
		}
		string value;

		virtual void SetLocalIdPass();
	};

	class Ternary : public Expression
	{
	public:
		Ternary(
			Expression* condition,
			Token questionToken,
			Expression* trueExpression,
			Expression* falseExpression)
			: Expression(TERNARY, condition->firstToken)
		{
			this->condition = condition;
			this->trueValue = trueExpression;
			this->falseValue = falseExpression;
			this->questionToken = questionToken;
		}
		Expression* condition;
		Expression* trueValue;
		Expression* falseValue;
		Token questionToken;

		virtual void SetLocalIdPass();
	};

	class Variable : public Expression
	{
	public:
		Variable(Token token, string name) : Expression(VARIABLE, token)
		{
			this->name = name;
		}
		string name;

		virtual void SetLocalIdPass();
	};
}

#endif
