#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sys/stat.h>

#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#else
#include <dirent.h>
#endif

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

	vector<wstring>* listDir(wstring path)
	{
		vector<wstring>* output = new vector<wstring>();
#ifdef WIN32
		WIN32_FIND_DATA data;
		HANDLE hFind;

		//const char * pathChar = path.c_str();
		//MultiByteToWideChar(CP_ACP, MB_COMPOSITE, path, -1, NULL, 0);


		hFind = FindFirstFile(path.c_str(), &data);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do {
				wstring s = wstring(data.cFileName);
				output->push_back(s);
			} while (FindNextFile(hFind, &data));
			FindClose(hFind);
		}

#else
		// This works on a Mac, from a snippet that said it worked on linux.
		// TODO: will need to do this on Windows, possibly #ifdef'ing a different solution.
		DIR *dpdf;
		struct dirent *epdf;

		dpdf = opendir(path.c_str());
		if (dpdf != NULL)
		{
			while ((epdf = readdir(dpdf)))
			{
				wstring s = wstring(epdf->d_name);
				if (!streq(s, ".") &&
					!streq(s, ".."))
				{
					output->push_back(s);
				}
			}
			closedir(dpdf);
		}

#endif
		// TODO: go through and remove common garbage paths
		// like .DS_Store, thumbs.db, .svn, .git*, etc.
		// Preferably here, outside of the #ifndef's

		return output;
	}

	bool isDirectory(string path)
	{
		struct stat s;
		if (stat(path.c_str(), &s) == 0)
		{
			if (s.st_mode & S_IFDIR)
			{
				return true;
			}
		}
		return false;
	}
}
