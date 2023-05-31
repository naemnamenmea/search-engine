#pragma once
#ifndef __search_engine__
#define __search_engine__

#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <memory>
#include <fstream>
#include <map>
#include <set>
#include <iostream>
#include <unordered_map>
#include <dirent.h>
#include <experimental/filesystem>


typedef std::multimap<std::size_t, std::string> SortedDocsByWord;
typedef std::unordered_map<std::string, SortedDocsByWord> WordFreqTable;

typedef std::unordered_map<std::string, std::size_t> FrequencyList;
typedef std::unordered_map<std::string, FrequencyList> DocsTable;

typedef std::vector<std::string> vString;

namespace fs = std::experimental::filesystem;


std::vector<std::string> getDirectoryFiles(const std::string& pathToDir);
std::string dirnameOf(const std::string& fname);



std::string getValueByKey(std::string key, vString args);
vString getValuesByKey(std::string key, vString args);
bool checkKey(std::string key, vString args);

class SearchEngine {
public:
	SearchEngine();
	SearchEngine(std::string path);
	typedef std::multimap<std::size_t, std::string> sortedFiles;

	bool addFile(const std::string& path, bool serialize);
	void addFolder(const std::string& path, bool serialize);
	sortedFiles search(std::vector<std::string> keys);
	void printSearchResault(sortedFiles files);
	static void showHelpMsg();
	void buildIndex(std::string path, bool expand, bool serialize = true);
	void setDataFolder(std::string path);
	std::string getDataFolder();
	std::string getIndexDFileName();
	std::string getIndexWFileName();
	std::string getCacheFileName();

	//friend std::ostream& operator<<(std::ostream&, FileCollection&);
private:
	std::string dataFolder;
	std::string sortedResult = "result.txt";
	std::string indexWFileName = "WIndex.txt";
	std::string indexDFileName = "DIndex.txt";
	std::string cacheFileName = "cache.txt";
	std::vector<std::string> words;
	FrequencyList caches;
	DocsTable filesTable;
	WordFreqTable wordsTable;

	size_t getValue(std::vector < std::string > keys, FrequencyList& wfl);
	void Serialize();
	void Serialize(FrequencyList& caches);
	void Serialize(DocsTable& filesTable);
	void Serialize(WordFreqTable& wordsTable);
	void deSerialize();
	vString isFilesUpToDate(vString);
	void wipe();

	//void Serialize(WordsFrequencyList& fr) {
	//	std::string path = dirnameOf(fr.path) + "\\" + filenameof(fr.path) + ".csv";
	//	std::ofstream os(fr.path);
	//}
	//
	//void deSerialize();

	//bool remDoc(std::string name);
	//void sort(std::string phrase);
};

#endif // __search_engine__


