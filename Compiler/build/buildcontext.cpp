#include "buildcontext.h"

#include <iostream>
#include <string>
#include <vector>
#include "../util/xmlnode.h"

using namespace std;

BuildContext::BuildContext()
{
	this->projectId = "";
	this->platform = "";
	this->source = "";
	this->output = "";
	this->jsFilePrefix = "";
	this->jsMinify = false;
	// TODO: resources
}

void buidlFileGetProjectId(XmlNode* target, BuildContext* context)
{
	string projectId = target->getStringValue("projectid");
	if (projectId.size() > 0) context->projectId = projectId;
}

void buildFileGetSource(XmlNode* target, BuildContext* context)
{
	string source = target->getStringValue("source");
	if (source.size() > 0) context->source = source;
}

void buildFileGetOutput(XmlNode* target, BuildContext* context)
{
	string output = target->getStringValue("output");
	if (output.size() > 0) context->output = output;
}

void buildFileGetPlatform(XmlNode* target, BuildContext* context)
{
	string platform = target->getStringValue("platform");
	if (platform.size() > 0) context->platform = platform;
}

void buildFileGetJsFilePrefix(XmlNode* target, BuildContext* context)
{
	string jsFilePrefix = target->getStringValue("jsfileprefix");
	if (jsFilePrefix.size() > 0) context->jsFilePrefix = jsFilePrefix;
}

void buildFileGetJsMinify(XmlNode* target, BuildContext* context)
{
	string jsMinify = target->getStringValue("minify");
	if (jsMinify.size() > 0) context->jsMinify = (jsMinify == "true");
}

/*
void vectorDump(vector<XmlNode*>* v)
{
	cout << "Vector(" << v->size() << "): ";
	for (int i = 0; i < v->size(); ++i) 
	{
		cout << "X ";
	}
	cout << endl;
}//*/

bool buildFileFindTarget(XmlNode* node, string buildTarget, vector<XmlNode*>* breadcrumb)
{
	breadcrumb->push_back(node);
	//vectorDump(breadcrumb);
	XmlNode* idNode = node->getValue("id");
	if (idNode != NULL) {
		if (buildTarget == idNode->getStringValue()) {
			// I found it!
			return true;
		}
	}

	vector<XmlNode*>* subTargets = node->getValues("target");
	if (subTargets != NULL)
	{
		for (int i = 0; i < subTargets->size(); ++i)
		{
			XmlNode* subTarget = subTargets->at(i);
			if (buildFileFindTarget(subTarget, buildTarget, breadcrumb))
			{
				return true;
			}
		}
	}
	breadcrumb->pop_back();
	return false;
}

void populateBuildContext(XmlNode* buildFile, string buildTarget, BuildContext* output)
{
	// DFS breadcrumb tracker to determine where the build target is.
	vector<XmlNode*> breadcrumb = vector<XmlNode*>();
	bool found = buildFileFindTarget(buildFile, buildTarget, &breadcrumb);
	if (!found)
	{
		throw "Build target not found.";
	}
	
	for (int i = 0; i < breadcrumb.size(); ++i)
	{
		XmlNode* node = breadcrumb.at(i);
		buidlFileGetProjectId(node, output);
		buildFileGetSource(node, output);
		buildFileGetOutput(node, output);
		buildFileGetPlatform(node, output);
		buildFileGetJsFilePrefix(node, output);
		buildFileGetJsMinify(node, output);
	}
}
