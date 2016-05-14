#include <string>
#include <unordered_map>
#include <vector>

#include "nodes.h"
#include "../tokens/token.h"
#include "../parser/exceptions.h"

using namespace std;
using namespace Nodes;

void Nodes::Assignment::SetLocalIdPass() { }
void Nodes::BinaryOpChain::SetLocalIdPass() { }
void Nodes::BooleanConstant::SetLocalIdPass() { }
void Nodes::BracketIndex::SetLocalIdPass() { }
void Nodes::BracketSlice::SetLocalIdPass() { }
void Nodes::ConstructorDefinition::SetLocalIdPass() { }
void Nodes::Dictionary::SetLocalIdPass() { }
void Nodes::DotField::SetLocalIdPass() { }
void Nodes::ExpressionAsExecutable::SetLocalIdPass() { }
void Nodes::FieldDefinition::SetLocalIdPass() { }
void Nodes::ForLoop::SetLocalIdPass() { }
void Nodes::FunctionDefinition::SetLocalIdPass() { }
void Nodes::FunctionInvocation::SetLocalIdPass() { }
void Nodes::InlineIncrement::SetLocalIdPass() { }
void Nodes::IntegerConstant::SetLocalIdPass() { }
void Nodes::IfStatement::SetLocalIdPass() { }
void Nodes::NullConstant::SetLocalIdPass() { }
void Nodes::StringConstant::SetLocalIdPass() { }
void Nodes::Ternary::SetLocalIdPass() { }
void Nodes::Variable::SetLocalIdPass() { }

Executable* Nodes::Assignment::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	this->targetExpression = this->targetExpression->ResolveNameReferences(lookup);
	this->valueExpression = this->valueExpression->ResolveNameReferences(lookup);
	return this;
}

Expression* Nodes::BinaryOpChain::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	for (int i = 0; i < this->expressions.size(); ++i)
	{
		this->expressions[i] = this->expressions[i]->ResolveNameReferences(lookup);
	}
	return this;
}

Expression* Nodes::BracketIndex::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	this->root = this->root->ResolveNameReferences(lookup);
	this->index = this->index->ResolveNameReferences(lookup);
	return this;
}

Expression* Nodes::BracketSlice::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	this->root->ResolveNameReferences(lookup);
	if (this->start != NULL) this->start = this->start->ResolveNameReferences(lookup);
	if (this->end != NULL) this->end = this->end->ResolveNameReferences(lookup);
	if (this->step != NULL) this->step = this->step->ResolveNameReferences(lookup);
	return this;
}

Executable* Nodes::ClassDefinition::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	// don't bother setting the output of any of these as they won't change.
	if (this->staticConstructor != NULL) this->staticConstructor->ResolveNameReferences(lookup);
	if (this->instanceConstructor != NULL) this->instanceConstructor->ResolveNameReferences(lookup);
	for (int i = 0; i < this->staticFieldDefinitions.size(); ++i)
	{
		this->staticFieldDefinitions[i]->ResolveNameReferences(lookup);
	}
	for (int i = 0; i < this->instanceFieldDefinitions.size(); ++i)
	{
		this->instanceFieldDefinitions[i]->ResolveNameReferences(lookup);
	}
	for (int i = 0; i < this->staticMethodDefinitions.size(); ++i)
	{
		this->staticMethodDefinitions[i]->ResolveNameReferences(lookup);
	}
	for (int i = 0; i < this->instanceMethodDefinitions.size(); ++i)
	{
		this->instanceMethodDefinitions[i]->ResolveNameReferences(lookup);
	}
	return this;
}

Executable* Nodes::ConstructorDefinition::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	for (int i = 0; i < this->argValues.size(); ++i)
	{
		Expression* argValue = this->argValues[i];
		if (argValue != NULL)
		{
			this->argValues[i] = argValue->ResolveNameReferences(lookup);
		}
	}
	for (int i = 0; i < this->baseArgs.size(); ++i)
	{
		this->baseArgs[i] = this->baseArgs[i]->ResolveNameReferences(lookup);
	}
	for (int i = 0; i < this->code.size(); ++i)
	{
		this->code[i] = this->code[i]->ResolveNameReferences(lookup);
	}
	return this;
}

Expression* Nodes::Dictionary::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	for (int i = 0; i < this->keys.size(); ++i)
	{
		this->keys[i] = this->keys[i]->ResolveNameReferences(lookup);
		this->values[i] = this->values[i]->ResolveNameReferences(lookup);
	}
	return this;
}

Expression* Nodes::DotField::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	// This one is going to be a little complicated.
	// Namespaces are thrown out completely by this point and so the full dot-field path
	// must be resolved and recursed inline here.
	throw new ParserException(this->firstToken, "TODO: dot field name resolution");
}

Executable* Nodes::ExpressionAsExecutable::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	this->expression = this->expression->ResolveNameReferences(lookup);
	return this;
}

Executable* Nodes::FieldDefinition::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	if (this->defaultValue != NULL)
	{
		this->defaultValue = this->defaultValue->ResolveNameReferences(lookup);
	}
	return this;
}

Executable* Nodes::ForLoop::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	for (int i = 0; i < this->init.size(); ++i)
	{
		this->init[i] = this->init[i]->ResolveNameReferences(lookup);
	}
	if (this->condition != NULL)
	{
		this->condition = this->condition->ResolveNameReferences(lookup);
	}
	for (int i = 0; i < this->step.size(); ++i)
	{
		this->step[i] = this->step[i]->ResolveNameReferences(lookup);
	}
	for (int i = 0; i < this->code.size(); ++i)
	{
		this->code[i] = this->code[i]->ResolveNameReferences(lookup);
	}
	return this;
}

Executable* Nodes::FunctionDefinition::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	for (int i = 0; i < this->argValues.size(); ++i)
	{
		Expression* arg = this->argValues[i];
		if (arg != NULL)
		{
			this->argValues[i] = arg->ResolveNameReferences(lookup);
		}
	}
	for (int i = 0; i < this->body.size(); ++i)
	{
		this->body[i] = this->body[i]->ResolveNameReferences(lookup);
	}
	return this;
}

Expression* Nodes::FunctionInvocation::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	this->root = this->root->ResolveNameReferences(lookup);
	for (int i = 0; i < this->args.size(); ++i)
	{
		this->args[i] = this->args[i]->ResolveNameReferences(lookup);
	}
	return this;
}

Executable* Nodes::IfStatement::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	this->condition = this->condition->ResolveNameReferences(lookup);
	for (int i = 0; i < this->trueCode.size(); ++i)
	{
		this->trueCode[i] = this->trueCode[i]->ResolveNameReferences(lookup);
	}

	for (int i = 0; i < this->falseCode.size(); ++i)
	{
		this->falseCode[i] = this->falseCode[i]->ResolveNameReferences(lookup);
	}
	return this;
}

Expression* Nodes::InlineIncrement::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	this->expression = this->expression->ResolveNameReferences(lookup);
	return this;
}

Expression* Nodes::Ternary::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	this->condition = this->condition->ResolveNameReferences(lookup);
	this->trueValue = this->trueValue->ResolveNameReferences(lookup);
	this->falseValue = this->falseValue->ResolveNameReferences(lookup);
	return this;
}

Expression* Nodes::Variable::ResolveNameReferences(const unordered_map<string, Executable*>& lookup)
{
	// This is one of the only ones that actually does interesting stuff.
	// Everything else is a recursive call to get down to the variables or dotFields.
	throw new ParserException(this->firstToken, "TODO: resolve variable");
}
