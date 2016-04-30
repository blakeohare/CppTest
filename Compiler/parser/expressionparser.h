#ifndef EXPRESSIONPARSER_H
#define EXPRESSIONPARSER_H

#include "nodes.h"
#include "../tokens/tokenstream.h"
#include "../tokens/token.h"

#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <vector>

using namespace std;
using namespace Nodes;
using namespace Tokens;

namespace Parser
{
	Expression* parseExpression(TokenStream* tokens);
}

#endif
