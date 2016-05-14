#ifndef EXECUTABLEPARSER_H
#define EXECUTABLEPARSER_H

#include "filecontext.h"
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
	void parseExecutables(TokenStream* tokens, vector<Executable*>* target, FileContext* fileContext);
	Executable* parseExecutable(TokenStream* tokens, FileContext* fileContext, bool allowComplex, bool semicolonExpected);
	//Expression* parseExpression(TokenStream* tokens, FileContext* fileContext);
	Executable* parseExpressionAsExecutable(TokenStream* tokens, FileContext* fileContext);
}

#endif
