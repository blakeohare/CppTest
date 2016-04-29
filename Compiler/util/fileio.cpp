#include <fstream>
#include <string>
#include <iostream>
#include <vector>
#include <sys/stat.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#include <tchar.h>
#else
#include <dirent.h>
#endif

#include "fileio.h"
#include "util.h"

using namespace std;
namespace FileIO {

	string readFileAsUtf8(string path)
	{
		string output = "";
		cout << "Reading file: " << path << endl;
		char buffer[100];
		FILE* pFile;
		char* content = (char*) malloc(sizeof(char) * 500);
		int contentVirtualLength = 0;
		int contentRealLength = 500;
		pFile = fopen(path.c_str(), "rb");

		int bytesRead;

		while (true) {
			bytesRead = fread(&buffer, sizeof(char), 100, pFile);
			cout << "Bytes read: " << bytesRead << endl;
			if (bytesRead > 0) {
				if (contentVirtualLength + bytesRead > contentRealLength) {
					int newRealLength = contentRealLength;
					while (contentVirtualLength + bytesRead > newRealLength) {
						newRealLength *= 2;
					}

					char* newContent = (char*) malloc(sizeof(char) * newRealLength);

					// TODO: look up memcpy when I land
					for (int i = 0; i < bytesRead; ++i) {
						newContent[i] = content[i];
					}
					free(content);
					content = newContent;
					contentRealLength = newRealLength;
				}
				// ...here too
				for (int i = 0; i < bytesRead; ++i) {
					content[contentVirtualLength + i] = buffer[i];
				}
				contentVirtualLength += bytesRead;
			} else {
				break;
			}
		}

		fclose(pFile);

		// content is now the raw bytes of the file.
		// TODO: Determine encoding and immediately convert to UTF8

		for (int i = 0; i < contentVirtualLength; ++i) {
			output += content[i];
		}

		/*
		UStringBuilder* sb = new UStringBuilder();
		string line;
		ifstream myfile(path);
		if (myfile.is_open())
		{
			while (getline(myfile, line))
			{
				sb.append(line);
				sb.append('\n');
			}
			myfile.close();
		}
		else
		{
			// TODO: throw exception
			cout << "??";
		}

		return output;//*/
		return output;
	}

	void listDirRecursiveHelper(
		const int rootLengthWithSlash,
		string currentPathWithSlash,
		vector<string>* output,
		const string fileExtension)
	{
		cout << "Current path with slash: " << currentPathWithSlash << endl;
		vector<string> rawFiles = vector<string>();
		listDir(currentPathWithSlash, &rawFiles);
		for (int i = 0; i < rawFiles.size(); ++i)
		{
			string fullPath = "" + currentPathWithSlash + rawFiles.at(i);
			cout << "Found: " << fullPath << endl;
			if (isDirectory(fullPath))
			{
				listDirRecursiveHelper(rootLengthWithSlash, fullPath + "/", output, fileExtension);
			}
			else if (stringEndsWith(fullPath, fileExtension))
			{
				string relativePath = substring(fullPath, rootLengthWithSlash);
				output->push_back(relativePath);
			}
		}
	}

	/*
		Lists all the files that match the file extension. Output vector contains
		all the file paths RELATIVE to the input root.
	*/
	void listDirRecursive(
		string root,
		vector<string>* output,
		string fileExtension)
	{
		if (!stringEndsWith(root, "/"))
		{
			root += "/";
		}
		int rootLength = root.size();

		if (fileExtension[0] != '.')
		{
			fileExtension = "." + fileExtension;
		}

		listDirRecursiveHelper(rootLength, root, output, fileExtension);
	}

	void listDir(string path, vector<string>* output)
	{
#ifdef WIN32
		WIN32_FIND_DATA data;
		HANDLE hFind;

		//const char * pathChar = path.c_str();
		//MultiByteToWideChar(CP_ACP, MB_COMPOSITE, path, -1, NULL, 0);


		hFind = FindFirstFile(path.c_str(), &data);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do
			{
				wstring s = wstring(data.cFileName);
				output.push_back(s);
			} while (FindNextFile(hFind, &data));
			FindClose(hFind);
		}

#else
		// This works on a Mac, from a snippet that said it worked on linux.
		DIR *dpdf;
		struct dirent *epdf;

		dpdf = opendir(path.c_str());
		if (dpdf != NULL)
		{
			while ((epdf = readdir(dpdf)))
			{
				string file = string(epdf->d_name);
				if (file != "." &&
					file != ".." &&
					file != ".svn" &&
					file != ".git" &&
					file != ".DS_Store" &&
					file != "thumbs.db")
				{
					output->push_back(file);
				}
			}
			closedir(dpdf);
		}
#endif
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

	string getParentDirectory(const string path)
	{
		int start = 0;
		int end = path.size() - 1;
		if (path[end] == '/')
		{
			end--;
		}

		bool found = false;
		while (end >= 0)
		{
			if (path[end--] == '/')
			{
				found = true;
				break;
			}
		}

		if (found)
		{
			return substring(path, 0, end + 1);
		}
		return ".";
	}
}
