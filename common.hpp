#pragma once

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include <map>
#include <set>
#include <windows.h>
#include <experimental\filesystem>
#include "hash.hpp"
#include "russian.hpp"

#define Error(string) throw std::exception(string)

namespace fs = std::experimental::filesystem;

struct FileInfo;

typedef unsigned int ui;
typedef std::vector<std::string> vString;
typedef std::vector<fs::path> vPath;
typedef std::multimap < ui, fs::path, std::greater<ui> > WordInDoc;
typedef std::unordered_map<std::string, WordInDoc> InvertedIndex;
typedef std::map<fs::path, FileInfo> FileList;
typedef std::unordered_map<std::string, ui> WordFreq;
typedef std::map<fs::path, ui> DocRank;

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

struct FileInfo
{
	fs::file_time_type last_modified;
	std::string hash;

	FileInfo() {}

	FileInfo(const fs::path& path)
	{
		last_modified = fs::last_write_time(path);
		hash = hashfile(path.string().c_str());
	}
};

void ColorMassege(const std::string& txt, const WORD color)
{
	SetConsoleTextAttribute(console, color);
	std::cout << txt << std::endl;
	SetConsoleTextAttribute(console, 15);
}

std::ostream& operator<<(std::ostream& os, const FileInfo& fi)
{
	std::time_t cftime = decltype(fi.last_modified)::clock::to_time_t(fi.last_modified);
	os << cftime << std::endl;
	os << fi.hash;
	return os;
}

std::istream& operator>>(std::istream& is, FileInfo& fi)
{
	std::time_t time;
	is >> time;
	fi.last_modified = fs::file_time_type::clock::from_time_t(time);
	is >> fi.hash;
	return is;
}

void FormatStr(std::string& str)
{
	char chars[] = "()!@#$%^&-*+=_\'\"|\\?/><,.:;{}[]";

	for (unsigned int i = 0; i < strlen(chars); ++i)
	{
		str.erase(std::remove(str.begin(), str.end(), chars[i]), str.end());
	}

	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
}

void FormateWord(std::string& word)
{
	word = Russian::Stemmer::Porter().stem(word);
}

bool endsWith(const std::string& s, const std::string& suffix)
{
	return s.size() >= suffix.size()
		&& s.substr(s.size() - suffix.size()) == suffix;
}

vString split(const std::string& s, const bool removeEmptyEntries = true, const std::string& delimiter = " ")
{
	vString tokens;

	for (size_t start = 0, end; start < s.length(); start = end + delimiter.length())
	{
		size_t position = s.find(delimiter, start);
		end = position != std::string::npos ? position : s.length();

		std::string token = s.substr(start, end - start);
		if (!removeEmptyEntries || !token.empty())
		{
			tokens.push_back(token);
		}
	}

	if (!removeEmptyEntries && (s.empty() || endsWith(s, delimiter)))
	{
		tokens.push_back("");
	}

	return tokens;
}

template <bool Recursive = false, class Func = function<void(fs::directory_entry)>>
void traverseDirectory(const fs::path& path, Func f)
{
	typename std::conditional<Recursive, fs::recursive_directory_iterator, fs::directory_iterator>::type  dir(path);
	for (auto &entry : dir) {
		f(entry);
	}
}