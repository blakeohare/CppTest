#include <vector>
#include "parser.h"
#include "nodes.h"
#include "../tokens/token.h"
#include "../tokens/tokenstream.h"

using namespace std;
using namespace Tokens;
using namespace nodes;

vector<Executable*>* parser::parseInterpretedCode(string rootFolder)
{
	return NULL;
}

vector<Executable*>* parser::parseExecutables(TokenStream* tokens)
{
	return NULL;
}

Executable* parser::parseExecutable(TokenStream* tokens)
{
	return NULL;
}

Expression* parser::parseExpression(TokenStream* tokens)
{
	return NULL;
}

Executable* parser::parseExpressionAsExecutable(TokenStream* tokens)
{
	return NULL;
}
