#ifndef NODES_H
#define NODES_H

#include <unordered_map>
#include <vector>

#include "../parser/filecontext.h"
#include "../tokens/token.h"

using namespace std;
using namespace Tokens;

class FileContext;
namespace Nodes
{

	enum NodeType
	{
		UNKNOWN_NODE,

		ASSIGNMENT,
		BINARY_OP_CHAIN,
		BOOLEAN_CONSTANT,
		BRACKET_INDEX,
		BRACKET_SLICE,
		CLASS_DEFINITION,
		CONSTRUCTOR_DEFINITION,
		DICTIONARY,
		DOT_FIELD,
		EXPRESSION_AS_EXECUTABLE,
		FIELD_DEFINITION,
		FOR_LOOP,
		FUNCTION_DEFINITION,
		FUNCTION_INVOCATION,
		IF_STATEMENT,
		IMPORT_STATEMENT,
		INLINE_INCREMENT,
		INTEGER_CONSTANT,
		NAMESPACE,
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

		ParseNode(NodeType type, Token firstToken, FileContext* fileContext) {
			this->type = type;
			this->firstToken = firstToken;
			this->fileContext = fileContext;
		}
		NodeType type;
		Token firstToken;
		Executable* owner;
		FileContext* fileContext;

		virtual void SetLocalIdPass() { };
	};

	class Executable : public ParseNode
	{
	public:
		Executable(NodeType type, Token firstToken, FileContext* fileContext)
			: ParseNode(type, firstToken, fileContext)
		{ }

		virtual void SetLocalIdPass() { };
		virtual Executable* ResolveNameReferences(const unordered_map<string, Executable*>& lookup) { return this; };
	};

	class Expression : public ParseNode
	{
	public:
		Expression(NodeType type, Token firstToken, FileContext* fileContext)
			: ParseNode(type, firstToken, fileContext)
		{ }

		virtual void SetLocalIdPass() { };
		virtual Expression* ResolveNameReferences(const unordered_map<string, Executable*>& lookup) { return this; };
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
			Expression* valueExpression,
			FileContext* fileContext)
			: Executable(ASSIGNMENT, targetExpression->firstToken, fileContext)
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
		virtual Executable* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class BinaryOpChain : public Expression
	{
	public:
		BinaryOpChain(vector<Expression*> expressions, vector<Token> ops, FileContext* fileContext)
			: Expression(BINARY_OP_CHAIN, expressions.at(0)->firstToken, fileContext)
		{
			this->expressions = expressions;
			this->ops = ops;
		}
		vector<Expression*> expressions;
		vector<Token> ops;

		virtual void SetLocalIdPass();
		virtual Expression* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class BooleanConstant : public Expression
	{
	public:
		BooleanConstant(Token token, bool value, FileContext* fileContext)
			: Expression(BOOLEAN_CONSTANT, token, fileContext)
		{
			this->value = value;
		}

		bool value;

		virtual void SetLocalIdPass();
	};

	class BracketIndex : public Expression
	{
	public:
		BracketIndex(Expression* root, Token bracketToken, Expression* index, FileContext* fileContext)
			: Expression(BRACKET_INDEX, root->firstToken, fileContext)
		{
			this->root = root;
			this->bracketToken = bracketToken;
			this->index = index;
		}

		Expression* root;
		Expression* index;
		Token bracketToken;

		virtual void SetLocalIdPass();
		virtual Expression* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class BracketSlice : public Expression
	{
	public:
		BracketSlice(
			Expression* root,
			Token bracketToken,
			Expression* start,
			Expression* end,
			Expression* step,
			FileContext* fileContext)
			: Expression(BRACKET_SLICE, root->firstToken, fileContext)
		{
			this->root = root;
			this->bracketToken = bracketToken;
			this->start = start;
			this->end = end;
			this->step = step;
		}

		Expression* root;
		Expression* start;
		Expression* end;
		Expression* step;
		Token bracketToken;

		virtual void SetLocalIdPass();
		virtual Expression* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class ClassDefinition : public Executable
	{
	public:
		ClassDefinition(
			Token classToken,
			Token nameToken,
			vector<Token> baseClassTokens,
			Executable* staticConstructor,
			vector<Executable*> staticFields,
			vector<Executable*> staticMethods,
			Executable* instanceConstructor,
			vector<Executable*> instanceFields,
			vector<Executable*> instanceMethods,
			FileContext* fileContext)
			: Executable(CLASS_DEFINITION, classToken, fileContext)
		{
			// TODO: other modifiers like abstract, final, and static
			this->classToken = classToken;
			this->nameToken = nameToken;
			this->name = nameToken.value;
			this->baseClassTokens = baseClassTokens;
			this->staticConstructor = staticConstructor;
			this->staticFieldDefinitions = staticFields;
			this->staticMethodDefinitions = staticMethods;
			this->instanceConstructor = instanceConstructor;
			this->instanceFieldDefinitions = instanceFields;
			this->instanceMethodDefinitions = instanceMethods;
		}

		Token firstToken;
		Token classToken;
		Token nameToken;
		string name;
		vector<Token> baseClassTokens;
		Executable* staticConstructor;
		vector<Executable*> staticFieldDefinitions;
		vector<Executable*> staticMethodDefinitions;
		Executable* instanceConstructor;
		vector<Executable*> instanceFieldDefinitions;
		vector<Executable*> instanceMethodDefinitions;

		virtual Executable* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class ConstructorDefinition : public Executable
	{
	public:
		ConstructorDefinition(
			Token firstToken,
			vector<Token> argNames,
			vector<Expression*> argValues,
			vector<Expression*> baseArgs,
			vector<Executable*> code,
			FileContext* fileContext)
			: Executable(CONSTRUCTOR_DEFINITION, firstToken, fileContext)
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
		virtual Executable* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class Dictionary : public Expression
	{
	public:
		Dictionary(
			Token firstToken,
			vector<Expression*> keys,
			vector<Expression*> values,
			FileContext* fileContext)
			: Expression(DICTIONARY, firstToken, fileContext)
		{
			this->keys = keys;
			this->values = values;
		}

		vector<Expression*> keys;
		vector<Expression*> values;

		virtual void SetLocalIdPass();
		virtual Expression* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class DotField : public Expression
	{
	public:
		DotField(Expression* root, Token dotToken, Token fieldNameToken, FileContext* fileContext)
			: Expression(DOT_FIELD, root->firstToken, fileContext)
		{
			this->root = root;
			this->dotToken = dotToken;
			this->fieldNameToken = fieldNameToken;
		}

		Expression* root;
		Token dotToken;
		Token fieldNameToken;

		virtual void SetLocalIdPass();
		virtual Expression* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class ExpressionAsExecutable : public Executable
	{
	public:
		ExpressionAsExecutable(Expression* expression, FileContext* fileContext)
			: Executable(EXPRESSION_AS_EXECUTABLE, expression->firstToken, fileContext)
		{
			this->expression = expression;
		}
		Expression* expression;
		virtual void SetLocalIdPass();
		virtual Executable* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class FieldDefinition : public Executable
	{
	public:
		FieldDefinition(Token firstToken, Token nameToken, Expression* value, FileContext* fileContext)
			: Executable(FIELD_DEFINITION, firstToken, fileContext)
		{
			this->isStatic = false;
			this->nameToken = nameToken;
			this->name = nameToken.value;
			this->defaultValue = value;
		}
		bool isStatic;
		Token nameToken;
		string name;
		Expression* defaultValue;

		virtual void SetLocalIdPass();
		virtual Executable* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class ForLoop : public Executable
	{
	public:
		ForLoop(
			Token forToken,
			vector<Executable*> init,
			Expression* condition,
			vector<Executable*> step,
			vector<Executable*> code,
			FileContext* fileContext)
			: Executable(FOR_LOOP, forToken, fileContext)
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
		virtual Executable* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
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
			vector<Executable*> body,
			FileContext* fileContext)
			: Executable(FUNCTION_DEFINITION, firstToken, fileContext)
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
		virtual Executable* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class FunctionInvocation : public Expression
	{
	public:
		FunctionInvocation(Expression* root, Token openParen, vector<Expression*> args, FileContext* fileContext)
			: Expression(FUNCTION_INVOCATION, root->firstToken, fileContext)
		{
			this->root = root;
			this->args = args;
			this->openParen = openParen;
		}
		Expression* root;
		Token openParen;
		vector<Expression*> args;

		virtual void SetLocalIdPass();
		virtual Expression* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class IfStatement : public Executable
	{
	public:
		IfStatement(
			Token ifToken,
			Expression* condition,
			vector<Executable*> trueCode,
			bool hasFalseCode,
			vector<Executable*> falseCode,
			FileContext* fileContext)
			: Executable(IF_STATEMENT, ifToken, fileContext)
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
		virtual Executable* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class ImportStatement : public Executable
	{
	public:
		ImportStatement(
			Token firstToken,
			vector<string> delimitedNames,
			FileContext* fileContext)
			: Executable(IMPORT_STATEMENT, firstToken, fileContext)
		{
			this->delimitedNames = delimitedNames;
		}
		vector<string> delimitedNames;
	};

	class InlineIncrement : public Expression
	{
	public:
		InlineIncrement(
			Token firstToken,
			Expression* expression,
			Token incrementToken,
			bool isPrefix,
			FileContext* fileContext)
			: Expression(INLINE_INCREMENT, firstToken, fileContext)
		{
			this->expression = expression;
			this->incrementToken = incrementToken;
			this->isPrefix = isPrefix;
		}
		Expression* expression;
		Token incrementToken;
		bool isPrefix;

		virtual void SetLocalIdPass();
		virtual Expression* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class IntegerConstant : public Expression
	{
	public:
		IntegerConstant(Token token, int value, FileContext* fileContext)
			: Expression(INTEGER_CONSTANT, token, fileContext)
		{
			this->value = value;
		}
		int value;

		virtual void SetLocalIdPass();
	};

	class Namespace : public Executable
	{
	public:
		Namespace(Token namespaceToken, Token nameToken, vector<Executable*> members, FileContext* fileContext)
			: Executable(NAMESPACE, namespaceToken, fileContext)
		{
			this->name = nameToken.value;
			this->members = members;
		}
		
		string name;
		vector<Executable*> members;

		// no need for resolution methods as this will get flattened out in the first pass
		// to create the global fully-qualified lookup of members.
	};

	class NullConstant : public Expression
	{
	public:
		NullConstant(Token token, FileContext* fileContext) : Expression(NULL_CONSTANT, token, fileContext) { }

		virtual void SetLocalIdPass();
	};

	class StringConstant : public Expression
	{
	public:
		StringConstant(Token token, string value, FileContext* fileContext)
			: Expression(STRING_CONSTANT, token, fileContext)
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
			Expression* falseExpression,
			FileContext* fileContext)
			: Expression(TERNARY, condition->firstToken, fileContext)
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
		virtual Expression* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};

	class Variable : public Expression
	{
	public:
		Variable(Token token, string name, FileContext* fileContext)
			: Expression(VARIABLE, token, fileContext)
		{
			this->name = name;
		}
		string name;

		virtual void SetLocalIdPass();
		virtual Expression* ResolveNameReferences(const unordered_map<string, Executable*>& lookup);
	};
}

#endif
