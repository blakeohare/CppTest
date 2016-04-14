#include <string>

#include "util.h"

using namespace std;

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
