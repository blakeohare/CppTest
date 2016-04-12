#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>

#include "tokenstream.h"

namespace Tokens
{
	TokenStream* tokenize(string* filename, string contents);
}

#endif
