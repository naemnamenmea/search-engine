#include <vector>
#include <string>
#include <dirent.h>
#include <iostream>

#include "se_aux.hpp"


std::string dirnameOf(const std::string& file)
{
	size_t pos = file.find_last_of("\\/");
	return (std::string::npos == pos) ? "" : file.substr(0, pos);
}

std::vector<std::string> getDirectoryFiles(const std::string& path) {
	std::vector<std::string> files;

	DIR *dir;
	struct dirent *ent;
	if ((dir = opendir(path.c_str())) != NULL) {
		while ((ent = readdir(dir)) != NULL) {
			const std::string str = ent->d_name;
			if (str.length() >= 4 && str.substr(str.length() - 4) == ".txt")
			{
				files.push_back(str);
			}
		}
		closedir(dir);
	}

	return files;
}

std::vector<std::string> split(const std::string& s)
{
	std::vector<std::string> ret;
	typedef std::string::size_type string_size;
	string_size i = 0;

	while (i != s.size()) {
		while (i != s.size() && isspace(s[i]))
			++i;

		string_size j = i;
		
		if (s[j] != s.size() && (s[j] == '\'' || s[j] == '\"')) {
			char quote = s[j];

			while (true) {
				++j;
				if (j == s.size() || s[j] == quote) break;
			}

			if (i + 1 != j - 1) {
				ret.push_back(s.substr(i + 1, j - i - 1));
				if (j == s.size()) break;
				else i = j + 1;
			}
		}
		else {
			while (j != s.size() && !isspace(s[j]))
				j++;

			if (i != j) {
				ret.push_back(s.substr(i, j - i));
				i = j;
			}
		}
	}
	return ret;
}