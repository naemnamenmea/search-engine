#include <sstream>
#include <string>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <iterator>
#include <memory>
#include <fstream>
#include <map>
#include <set>
#include <iostream>
#include <unordered_map>
#include <codecvt>
#include <locale>
#include <experimental/filesystem>
#include <regex>
#include <dirent.h>

#include "search_engine.hpp"


SearchEngine::SearchEngine() {
	SearchEngine(getDataFolder());
}

SearchEngine::SearchEngine(std::string path)
{
	setDataFolder(path);
	
}

void SearchEngine::showHelpMsg() {

}

void SearchEngine::addFolder(const std::string& path, bool serialize) {
	std::vector<std::string> files = getDirectoryFiles(path);

	for (const auto& file : files) {
		if (addFile(path + file, false)) {
			std::cout << "added: \"" + file + "\"" << std::endl;
		}
		else {
			std::cout << "error: \"" << file << "\"" << std::endl;
		}		
	}
	if (serialize) Serialize();	
}

SearchEngine::sortedFiles SearchEngine::search(std::vector<std::string> keys) {
	sortedFiles files;

	for (auto& doc : filesTable) {
		files.insert(std::make_pair(getValue(keys, doc.second), doc.first));
	}

	return files;
};

 bool SearchEngine::addFile(const std::string& path, bool serialize) {
	std::string word;
	std::ifstream ifs(path);
	if (!ifs) return false;

	FrequencyList wfl;

	while (ifs >> word) {
		std::transform(word.begin(), word.end(), word.begin(), ::tolower);
		if (wfl.find(word) != wfl.end()) {
			++wfl[word];
		}
		else {
			wfl.insert(std::make_pair(word, 1));
		}
		
		wordsTable[word].insert(std::make_pair(wfl[word],path));
	}
	ifs.close();

	filesTable.insert(std::make_pair(path,wfl));

	if (serialize) Serialize();
	
	return true;
}

size_t SearchEngine::getValue(std::vector < std::string > keys, FrequencyList& wfl) {
	size_t value = 0;

	for (auto& word : keys) {
		value += wfl[word];
	}
	return value;
}

void SearchEngine::Serialize() {
	Serialize(caches);
	Serialize(wordsTable);
	Serialize(filesTable);
}

void SearchEngine::Serialize(FrequencyList & caches)
{
	std::ofstream ofs(cacheFileName);

	for (auto& doc : caches) {
		ofs << doc.first << std::endl;
		ofs << doc.second << std::endl;
	}

	ofs.close();
}

void SearchEngine::Serialize(DocsTable& docsTable) {
	std::ofstream ofs(indexDFileName);
	
	for (auto& doc : docsTable) {
		ofs << doc.first << std::endl;
		ofs << doc.second.size() << std::endl;

		for (auto& word : doc.second) {
			ofs << word.first << std::endl;
			ofs << word.second << std::endl;
		}
	}

	ofs.close();
}

void SearchEngine::Serialize(WordFreqTable& wft) {
	std::ofstream ofs(indexWFileName);

	for (auto& word : wordsTable) {
		ofs << word.first << std::endl;
		ofs << word.second.size() << std::endl;

		for (auto& doc : word.second) {
			ofs << doc.first << std::endl;
			ofs << doc.second << std::endl;
		}
	}

	ofs.close();
}

void SearchEngine::deSerialize() {
	std::string in, item;
	size_t len, w;
	std::ifstream ifs;

	ifs.open(indexWFileName);	
	WordFreqTable wft;
	while (!ifs.eof()) {
		SortedDocsByWord sdbw;
		std::getline(ifs, item);
		ifs >> len;

		for (size_t i = 0; i < len; i++) {
			std::getline(ifs, in);
			ifs >> w;
			sdbw.insert(std::make_pair(w, in));
		}
		wft.insert(std::make_pair(item, sdbw));
	}
	ifs.close();

	ifs.open(indexDFileName);
	DocsTable dt;
	while (!ifs.eof()) {
		FrequencyList fl;
		std::getline(ifs, item);
		ifs >> len;
		for (size_t i = 0; i < len; i++) {
			std::getline(ifs, in);
			ifs >> w;
			fl.insert(std::make_pair(in, w));
		}
		dt.insert(std::make_pair(item, fl));
	}
	ifs.close();

	ifs.open(cacheFileName);
	while (!ifs.eof()) {
		std::getline(ifs, in);
		ifs >> w;
		caches.insert(std::make_pair(in,w));
	}
	ifs.close();
}

void SearchEngine::buildIndex(std::string path, bool expand, bool serialize) {
	struct stat s;
	if (stat(path.c_str(), &s) == 0)
	{
		if(!expand)	wipe();		

		if (s.st_mode & S_IFDIR || s.st_mode & S_IFREG)
		{
			vString files;

			if (s.st_mode & S_IFDIR)
				files = getDirectoryFiles(path);
			else
				files = vString{path};

			bool update = isFilesUpToDate(files).size() == 0;

			if (s.st_mode & S_IFDIR) {				
				if (update) addFolder(path, serialize);
			}
			else  {
				if (update)	addFile(path, serialize);
			}

			//if (!update) deSerialize();
		}
		else
		{
			std::cout << "something else" << std::endl;
		}
	}
	else
	{
		std::cout << "Wrong path: " + path << std::endl;
	}
}

void SearchEngine::setDataFolder(std::string path)
{
	dataFolder = path;
}

std::string SearchEngine::getDataFolder()
{
	return dataFolder;
}

std::string SearchEngine::getIndexDFileName()
{
	return indexDFileName;
}

std::string SearchEngine::getIndexWFileName()
{
	return indexWFileName;
}

std::string SearchEngine::getCacheFileName()
{
	return cacheFileName;
}

vString SearchEngine::isFilesUpToDate(vString) {
	//std::string  filename = "mplAux.hpp";
	//struct stat result;
	//if (stat(filename.c_str(), &result) == 0)
	//{
	//	auto mod_time = result.st_mtime;
	//	std::cout << std::endl << mod_time;
	//}
	return vString();
}

void SearchEngine::wipe()
{
	caches.clear();
	filesTable.clear();
	wordsTable.clear();
	remove("WIndex.txt");
	remove("DIndex.txt");
	remove("cache.txt");
}

std::string getValueByKey(std::string key, vString args)
{
	auto it = std::find(args.begin(), args.end(), key);
	if (it != args.end() && ++it != args.end())
	{
		return *it;
	}
	else
	{
		return "";
	}
}

vString getValuesByKey(std::string key, vString args)
{
	vString values;
	auto it = std::find(args.begin(), args.end(), key);
	if (it != args.end())
	{
		while (++it != args.end() && (*it).find("--") != 0) {
			values.push_back(*it);
		}
	}

	return values;
}


bool checkKey(std::string key, vString args)
{
	return std::find(args.begin(), args.end(), key) != args.end();
}