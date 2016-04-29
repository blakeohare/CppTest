#include <string>
#include <vector>

using namespace std;

namespace FileIO {
	string readFileAsUtf8(string path);
	void listDir(string path, vector<string>* output);
	bool isDirectory(string path);
	void listDirRecursive(string root, vector<string>* output, string fileExtension);
	string getParentDirectory(const string path);
}