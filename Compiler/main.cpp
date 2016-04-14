#include <iostream>
#include <unordered_map>
#include <vector>
#include <string>
#include <string.h>

#include "tokens/tokenizer.h"
#include "tokens/token.h"
#include "tokens/tokenstream.h"
#include "util/fileio.h"
#include "util/xmlnode.h"

using namespace std;
using namespace Tokens;

int main()
{
	string filename = "main.cry";
	string file = FileIO::readFile(filename);
	TokenStream* tokenstream = tokenize(filename, file);

	cout << tokenstream->length << endl;

	while (tokenstream->hasMore())
	{
		Token* token = tokenstream->pop();
		string value = token->value;
		//cout << *value << endl;
	}

	XmlNode* buildFileNode = parseXmlDocument("Test.build", "build");

	XmlNode* targetNode = buildFileNode->getValue("target");
	cout << buildFileNode->getValue("target")->getValue("platform")->getStringValue() << endl;

	vector<string>* files = FileIO::listDir("./");
	for (int i = 0; i < files->size(); ++i)
	{
		cout << "File: " << files->at(i) << endl;
	}

	return 0;
}
