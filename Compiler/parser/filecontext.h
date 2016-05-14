#ifndef FILE_CONTEXT_H
#define FILE_CONTEXT_H

#include <string>
#include <vector>

#include "../parser/nodes.h"

using namespace std;

namespace Nodes
{
	class ImportStatement;
}

class FileContext
{
public:
	FileContext(string filename)
	{
		this->filename = filename;
		this->imports = vector<Nodes::ImportStatement*>();
	}

	string filename;
	vector<Nodes::ImportStatement*> imports; 
};

#endif
