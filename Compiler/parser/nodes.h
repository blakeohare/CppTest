#ifndef NODES_H
#define NODES_H

#include <vector>

#include "../tokens/token.h"

using namespace std;
using namespace Tokens;

namespace nodes
{
	class Executable;

	class ParseNode
	{
	public:
		ParseNode(Token* firstToken, Executable* owner) {
		this->firstToken = firstToken;
		this->owner = owner;
	}
		Token* firstToken;
		Executable* owner;
		virtual void SetLocalIdPass();
	};

	class Executable : public ParseNode
	{
	public:
		Executable(Token* firstToken, Executable* owner)
			: ParseNode(firstToken, owner)
		{
			this->firstToken = firstToken;
			this->owner = owner;
		}
	};

	class Expression : public ParseNode
	{
	public:
		Expression(Token* firstToken, Executable* owner)
			: ParseNode(firstToken, owner)
		{ }
	};

	class IfStatement : public Executable
	{
	public:
		IfStatement(
			Token* ifToken, 
			Executable* owner,
			Expression* condition,
			vector<Executable*>* trueCode,
			vector<Executable*>* falseCode)
			: Executable(ifToken, owner)
		{
			this->condition = condition;
			this->trueCode = trueCode;
			this->falseCode = falseCode;
		}
		Expression* condition;
		vector<Executable*>* trueCode;
		vector<Executable*>* falseCode;
	};
}

#endif
