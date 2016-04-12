#include <string>
#include <vector>

using namespace std;

namespace FileIO {
	string readFile(string path);
	vector<string> listDir(string path);
	bool isDirectory(string path);
}