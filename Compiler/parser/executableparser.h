#ifndef EXECUTABLEPARSER_H
#define EXECUTABLEPARSER_H

#include "nodes.h"
#include "../tokens/tokenstream.h"
#include "../tokens/token.h"

#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
using namespace Nodes;
using namespace Tokens;

namespace Parser
{
	vector<Executable*>* parseInterpretedCode(string rootFolder);
	void parseExecutables(TokenStream* tokens, vector<Executable*>* target);
	Executable* parseExecutable(TokenStream* tokens, bool allowComplex, bool semicolonExpected);
	Expression* parseExpression(TokenStream* tokens);
	Executable* parseExpressionAsExecutable(TokenStream* tokens);
}

#endif
