#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include "tinyxml2.h"
#include "xmlnode.h"

using namespace std;


XmlNode::XmlNode()
{
	this->stringValue = "";
	this->values = NULL;
}

string XmlNode::getStringValue()
{
	return this->stringValue;
}

XmlNode* XmlNode::getValue(string key)
{
	if (this->values == NULL) return NULL;
	if (this->values->count(key) == 0) return NULL;
	return this->values->at(key)->at(0);
}

string XmlNode::getStringValue(string key)
{
	XmlNode* node = this->getValue(key);
	if (node != NULL)
	{
		return node->getStringValue();
	}
	return "";
}

vector<XmlNode*>* XmlNode::getValues(string key)
{
	if (this->values == NULL) return NULL;
	if (this->values->count(key) == 0) return NULL;
	return this->values->at(key);
}

void XmlNode::setStringValue(string value)
{
	this->stringValue = value;
}

void XmlNode::setValue(string key, XmlNode* value)
{
	if (this->values == NULL)
	{
		this->values = new unordered_map<string, vector<XmlNode*>*>();
	}

	if (this->values->count(key) == 0)
	{
		this->values->insert(pair<string, vector<XmlNode*>*>(key, new vector<XmlNode*>()));
	}

	this->values->at(key)->push_back(value);
}

XmlNode* parseXmlElement(tinyxml2::XMLElement* element)
{
	XmlNode* output = new XmlNode();
	// iterate through attributes
	const tinyxml2::XMLAttribute* attrib = element->FirstAttribute();

	while (attrib)
	{
		XmlNode* attributeNode = new XmlNode();
		attributeNode->setStringValue(attrib->Value());
		output->setValue(attrib->Name(), attributeNode);
		attrib = attrib->Next();
	}

	// iterate through children while may either be more elements or text values.
	tinyxml2::XMLNode* child = element->FirstChild();
	
	while (child)
	{
		if (child->ToElement() != NULL)
		{
			output->setValue(child->ToElement()->Name(), parseXmlElement(child->ToElement()));
		}
		else if (child->ToText() != NULL)
		{
			// Set the text value. If there are multiple then it will overwrite with the last one.
			// If there is only one, as there should be, this is fine.
			// TODO: error checking if there is extraneous non-whitespace stuff floating around.
			// Mixed elements+text shouldn't be allowed.
			output->setStringValue(child->Value());
		}
		child = child->NextSibling();
	}

	return output;
}

XmlNode* parseXmlDocument(const char* pFilename, const char* expectedRoot)
{
	tinyxml2::XMLDocument doc;
	tinyxml2::XMLError error = doc.LoadFile(pFilename);
	cout << error << endl;
	if (error != tinyxml2::XML_SUCCESS)
	{
		cout << "Was not able to load document." << endl;
		return NULL;
	}

	tinyxml2::XMLNode* root = doc.FirstChild();
	while (root != NULL)
	{
		if (root->ToElement() != 0)
		{
			cout << root->Value() << " " << expectedRoot << endl;
			if (strcmp(root->Value(), expectedRoot) == 0)
			{
				return parseXmlElement(root->ToElement());
			}
		}
		root = root->NextSibling();
	}
	cout << "Document did not have expected root." << endl;
	return NULL;
}
