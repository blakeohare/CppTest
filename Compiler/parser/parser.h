#ifndef PARSER_H
#define PARSER_H

#include "parser.h"
#include "nodes.h"
#include "../tokens/tokenstream.h"
#include "../tokens/token.h"

#include <vector>

using namespace std;
using namespace nodes;
using namespace Tokens;

namespace parser
{
	vector<Executable*>* parseInterpretedCode(string rootFolder);
	vector<Executable*>* parseExecutables(TokenStream* tokens);
	Executable* parseExecutable(TokenStream* tokens);
	Expression* parseExpression(TokenStream* tokens);
	Executable* parseExpressionAsExecutable(TokenStream* tokens);
}

#endif
