#ifndef XMLNODE_H
#define XMLNODE_H

#include <string>
#include <vector>
#include <unordered_map>
#include "tinyxml2.h"

using namespace std;

class XmlNode
{
public:
	XmlNode();
	void setStringValue(string value);
	void setValue(string key, XmlNode* value);
	string getStringValue();
	XmlNode* getValue(string key);
	vector<XmlNode*>* getValues(string key);
private:
	string stringValue;
	unordered_map<string, vector<XmlNode*>*>* values;
};

XmlNode* parseXmlDocument(const char* pFilename, const char* expectedRoot);

#endif
