#include <iostream>
#include "tokens/tokenizer.h"
#include "util/fileio.h"

using namespace std;
using namespace Tokens;

int main()
{
	string* filename = new string("main.cry");
	string file = FileIO::readFile(*filename);
	TokenStream* tokenstream = tokenize(filename, file);

	cout << tokenstream->length << endl;

	while (tokenstream->hasMore())
	{
		Token* token = tokenstream->pop();
		string* value = token->value;
		cout << *value << endl;
	}

	return 0;
}
