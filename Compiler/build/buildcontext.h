#ifndef BUILDCONTEXT_H
#define BUILDCONTEXT_H

#include <string>
#include "../util/xmlnode.h"

using namespace std;

class BuildContext
{
public:
	BuildContext();
	string source;
	string output;
	string platform;
	string projectId;
	string jsFilePrefix;
	bool jsMinify;
};

void populateBuildContext(XmlNode* buildFile, string buildTarget, BuildContext* output);

#endif
