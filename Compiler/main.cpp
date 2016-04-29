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
#include "build/buildcontext.h"
#include "parser/nodes.h"
#include "parser/parser.h"

using namespace std;
using namespace Tokens;
using namespace Nodes;

int main(const int argc, const char** argv)
{
	// crayon
	if (argc == 1) 
	{
		cout << "Crayon 0.2.1" << endl;
		cout << "For usage try crayon -help" << endl;
		cout << "For tutorials, go to http://crayonlang.org" << endl;
		return 0;
	}

	if (argc == 2 && strcmp(argv[1], "-help") == 0)
	{
		cout << "TODO: usage" << endl;
		return 0;
	}

	// crayon build_file -target id
	if (argc == 4 && strcmp(argv[2], "-target") == 0)
	{
		string buildFilePath = string(argv[1]);
		string buildTargetId = string(argv[3]);
		XmlNode* buildFileNode = parseXmlDocument(buildFilePath.c_str(), "build");
		BuildContext buildContext = BuildContext();
		populateBuildContext(buildFileNode, buildTargetId, &buildContext);

		string source = buildContext.source;
		string sourceRelativePath = FileIO::getParentDirectory(buildFilePath) + "/" + source;
		vector<string> files = vector<string>();
		FileIO::listDirRecursive(sourceRelativePath, &files, "cry");
		vector<Executable*> executables = vector<Executable*>();
		cout << "Files found: " << files.size() << endl;
		for (int i = 0; i < files.size(); ++i)
		{
			cout << "File: " << files.at(i) << endl;
			string fullFilePath = "./" + sourceRelativePath + files.at(i);
			cout << "Full path: " << fullFilePath << endl;
			string contents = FileIO::readFileAsUtf8(fullFilePath);
			TokenStream tokens = TokenStream();
			cout << "Contents: " << endl << contents << endl;
			Tokens::tokenize(files.at(i), contents, &tokens);
			Parser::parseExecutables(&tokens, &executables);
		}
	}

	/*
	string filename = "main.cry";
	string filecontents = "";
	FileIO::readFileAsUtf8(filename, &filecontents);
	TokenStream* tokenstream = tokenize(filename, filecontents);

	cout << tokenstream->length << endl;

	while (tokenstream->hasMore())
	{
		Token* token = tokenstream->pop();
		string value = token->value;
		//cout << *value << endl;
	}

	XmlNode* buildFileNode = parseXmlDocument("Test.build", "build");

	XmlNode* targetNode = buildFileNode->getValue("target");
	//cout << buildFileNode->getValue("target")->getValue("platform")->getStringValue() << endl;

	BuildContext buildContext = BuildContext();
	//cout << "WAT" << endl;
	populateBuildContext(buildFileNode, "windows", &buildContext);

	cout << "Project ID: " << buildContext.projectId << endl;
	cout << "Source: " << buildContext.source << endl;
	cout << "Output: " << buildContext.output << endl;
	cout << "JS File Prefix: " << buildContext.jsFilePrefix << endl;
	cout << "Platform: " << buildContext.platform << endl;
	cout << "JS Minify: " << buildContext.jsMinify << endl;

	string path = "./";
	vector<string> files;
	FileIO::listDir(path, &files);
	
	for (unsigned int i = 0; i < files.size(); ++i)
	{
		string s = files.at(i);
		cout << "File: " << s << endl;
	}
	//*/

	return 0;
}
