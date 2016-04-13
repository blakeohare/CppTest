#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <string.h>

#include "tokens/tokenizer.h"
#include "util/fileio.h"
#include "util/xmlnode.h"

using namespace std;

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
		//cout << *value << endl;
	}

	XmlNode* buildFileNode = parseXmlDocument("Test.build", "build");

	XmlNode* targetNode = buildFileNode->getValue("target");
	cout << buildFileNode->getValue("target")->getValue("platform")->getStringValue() << endl;

	return 0;
}
