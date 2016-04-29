#include <string>

#include "util.h"

using namespace std;

bool string2Integer(const string value, int* valueOut)
{
	int output = 0;
	char c;
	for (int i = 0; i < value.size(); ++i)
	{
		c = value[0];
		if (c >= '0' && c <= '9')
		{
			output = output * 10 + (c - '0');
		}
		else
		{
			return false;
		}
	}
	*valueOut = output;
	return true;
}

void utf16toUtf8(wstring ws, string* output)
{
	// TODO: look up wstring methods and UTF16 encoding
}

bool stringEndsWith(const string& str, const string& suffix)
{
	if (suffix.size() > str.size()) return false;
	for (int i = 0; i < suffix.size(); ++i)
	{
		if (str[str.size() - i] != suffix[suffix.size() - i])
		{
			return false;
		}
	}
	return true;
}

string substring(const string str, const int startIndex)
{
	return substring(str, startIndex, str.size() - startIndex);
}

string substring(const string str, const int startIndex, const int length)
{
	string output = "";
	for (int j = 0; j < length && startIndex + j < str.size(); ++j)
	{
		output += str[startIndex + j];
	}
	return output;
}

/*
bool streq(const char* a, const char* b)
{
	return strcmp(a, b) == 0;	
}

bool streq(string a, string b)
{
	return streq(a.c_str(), b.c_str());
}

bool streq(string* a, string* b)
{
	return streq(a->c_str(), b->c_str());
}

bool streq(wstring a, string b)
{
	if (a.size() != b.size()) return false;
	for (int i = a.size() - 1; i >= 0; --i)
	{
		if ((int) a[i] != (int) b[i])
		{
			return false;
		}
	}
	return true;
}


int ustrlen(ustring s)
{
	return s[-1];
}

ustring new_ustring(string s)
{
	UStringBuilder sb;
	sb.append(s);
	return sb.toString();
}

free_string(ustring s)
{
	free(s - 1);
}//*/

