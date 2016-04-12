#include <fstream>
#include <string>
#include <iostream>
#include <vector>

#include "fileio.h"

using namespace std;
namespace FileIO {

	string readFile(string path)
	{
		string output = "";
		string line;
		ifstream myfile(path);
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				output.append(line);
				output.append("\n");
			}
			myfile.close();
		}
		else
		{
			// TODO: throw exception
			cout << "??";
		}

		return output;
	}

	vector<string> listDir(string path)
	{
		// TODO: this
		vector<string> output;
		return output;
	}

	bool isDirectory(string path)
	{
		return true;
	}
}
