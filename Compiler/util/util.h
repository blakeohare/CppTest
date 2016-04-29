#include <string>
#include <stdlib.h>

using namespace std;

bool string2Integer(const string value, int* valueOut);
void utf16toUtf8(wstring ws, string* output);

// TODO: Look up existing implementations when you land.
bool stringEndsWith(const string& str, const string& suffix);
string substring(const string str, const int startIndex);
string substring(const string str, const int startIndex, const int length);

//bool streq(const char* a, const char* b);
//bool streq(string a, string b);
//bool streq(string* a, string* b);
//bool streq(wstring a, string b);

//typedef int* ustring;

//bool streq(ustring a, string b);

//int ustrlen(ustring s);
//ustring new_ustring(string s);
//free_string(ustring s);
