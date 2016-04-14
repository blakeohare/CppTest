#include <string>
#include <vector>

using namespace std;

namespace FileIO {
	string readFile(string path);
	vector<wstring>* listDir(wstring path);
	bool isDirectory(string path);
}