#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <dirent.h>
#include <sys/stat.h>

#include "fileio.h"
#include "../util/util.h"

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

	vector<string>* listDir(string path)
	{
		// This works on a Mac, from a snippet that said it worked on linux.
		// TODO: will need to do this on Windows, possibly #ifdef'ing a different solution.
		vector<string>* output = new vector<string>();

		DIR *dpdf;
		struct dirent *epdf;

		dpdf = opendir(path.c_str());
		if (dpdf != NULL)
		{
			while ((epdf = readdir(dpdf)))
			{
				string s = string(epdf->d_name);
				if (!streq(s, ".") &&
					!streq(s, ".."))
				{
					output->push_back(s);
				}
			}
			closedir(dpdf);
		}

		// TODO: go through and remove common garbage paths
		// like .DS_Store, thumbs.db, .svn, .git*, etc.

		return output;
	}

	bool isDirectory(string path)
	{
		struct stat s;
		if (stat(path, &s) == 0)
		{
			if (s.st_mode & S_IFDIR)
			{
				return true;
			}
		}
		return false;
	}
}
