#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "../parser/exceptions.h"
#include "../parser/filecontext.h"
#include "../parser/nodes.h"
#include "resolver.h"

using namespace std;
using namespace Nodes;
using namespace Resolver;

namespace Resolver
{
	void generateFullyQualifiedLookupImpl(
		vector<Executable*> code,
		string prefix,
		unordered_map<string, Executable*>* lookup,
		vector<string>* allNames,
		FunctionDefinition** mainFunction);


	void verifyMainFunction(const FunctionDefinition* mainFunction)
	{
		if (mainFunction == NULL)
			throw new ParserException("No function found named 'main'. A main function is required as the point of entry of execution.");
		if (mainFunction->argNames.size() > 1)
			throw new ParserException(mainFunction->firstToken, "The 'main' function must accept either 0 or 1 arguments.");
	}

	void resolveNameReferences(
		const vector<string>& allQualifiedNames,
		const vector<Executable*>& allItems,
		const unordered_map<string, Executable*>& lookup);

	void resolveCode(vector<Executable*> code)
	{
		unordered_map<string, Executable*> lookupByQualifiedName = 
			unordered_map<string, Executable*>();
		vector<string> allQualifiedNames = vector<string>();
		vector<Executable*> allItems = vector<Executable*>();
		FunctionDefinition* mainFunction = NULL;
		generateFullyQualifiedLookupImpl(
			code, "", &lookupByQualifiedName, &allQualifiedNames,
			&mainFunction);

		for (int i = 0; i < allQualifiedNames.size(); ++i)
		{
			cout << allQualifiedNames.at(i) << endl;
			string name = allQualifiedNames.at(i);
			unordered_map<string, Executable*>::const_iterator ex = lookupByQualifiedName.find(name);
			allItems.push_back(ex->second);
			

		}

		cout << "Verifying main" << endl;
		verifyMainFunction(mainFunction);
		
		cout << "resolve name references" << endl;

		resolveNameReferences(allQualifiedNames, allItems, lookupByQualifiedName);
	}

	void generateFullyQualifiedLookupImpl(
		vector<Executable*> code,
		string prefix,
		unordered_map<string, Executable*>* lookup,
		vector<string>* allNames,
		FunctionDefinition** mainFunction)
	{
		string name;
		bool isNamespace;
		for (int i = 0; i < code.size(); ++i)
		{
			isNamespace = false;
			Executable* codeItem = code.at(i);
			if (codeItem->type == FUNCTION_DEFINITION)
			{
				FunctionDefinition* funcDef = (FunctionDefinition*) codeItem;
				name = "" + prefix + funcDef->name;
				if (funcDef->name == "main")
				{
					if (*mainFunction == NULL)
					{
						*mainFunction = funcDef;
					}
					else
					{
						throw new ParserException(funcDef->firstToken, 
							"Multiple functions named 'main'");
					}
				}
			}
			else if (codeItem->type == FIELD_DEFINITION)
			{
				FieldDefinition* fieldDef = (FieldDefinition*) codeItem;
				name = "" + prefix + fieldDef->name;
			}
			else if (codeItem->type == CLASS_DEFINITION)
			{
				ClassDefinition* classDef = (ClassDefinition*) codeItem;
				name = "" + prefix + classDef->name;
				string newPrefix = name + ".";
				generateFullyQualifiedLookupImpl(classDef->staticFieldDefinitions, newPrefix, lookup, allNames, mainFunction);
				generateFullyQualifiedLookupImpl(classDef->instanceFieldDefinitions, newPrefix, lookup, allNames, mainFunction);
				generateFullyQualifiedLookupImpl(classDef->staticMethodDefinitions, newPrefix, lookup, allNames, mainFunction);
				generateFullyQualifiedLookupImpl(classDef->instanceMethodDefinitions, newPrefix, lookup, allNames, mainFunction);
			}
			else if (codeItem->type == NAMESPACE)
			{
				isNamespace = true;
				Namespace* namespaceDef = (Namespace*) codeItem;
				name = "" + prefix + namespaceDef->name;
				string newPrefix = "" + name + ".";
				generateFullyQualifiedLookupImpl(namespaceDef->members, newPrefix, lookup, allNames, mainFunction);
			}
			else
			{
				string errorMessage = 
					"This sort of expression cannot exist outside of function or field definitions.";
				if (codeItem->type == ASSIGNMENT)
				{
					errorMessage += " Did you mean to mark this as a const expression?";
				}
				throw new ParserException(codeItem->firstToken, errorMessage);
			}

			unordered_map<string, Executable*>::const_iterator collision = lookup->find(name);
			if (collision != lookup->end())
			{
				if (collision->second->type != NAMESPACE ||
					codeItem->type != NAMESPACE)
				{
					throw new ParserException(codeItem->firstToken, "This item's name collides with another entity.");
				}
			}

			lookup->insert(make_pair(name, codeItem));
			allNames->push_back(name);
		}
	}

	/*
		Convert all name-only references into resolved references.
			- arbitrary variable -> function pointer or class name
			- dot fields -> static methods/fields
			- resolve new-keyword nodes to point to their class node
			- subclasses/interfaces point to their class node

		This requires knowledge of which imports occurred at the top of the file.
	*/
	void resolveNameReferences(
		const vector<string>& allQualifiedNames,
		const vector<Executable*>& allItems,
		const unordered_map<string, Executable*>& lookup)
	{
		for (int i = 0; i < allItems.size(); ++i)
		{
			allItems.at(i)->ResolveNameReferences(lookup);
		}
	}
}
