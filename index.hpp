#pragma once

#include <iostream>
#include <string>
#include <limits>
#include <stdio.h>
#include <fstream>
#include <sys/stat.h>
#include "common.hpp"
#include "russian.hpp"
#include "hash.hpp"

#define SE_SERVICE_ROOT_PATH "C:\\SE_service_data"
#define SE_EXIT1 "exit"
#define SE_EXIT2 "quit"
#define SE_HELP "help"
#define SE_WIPE1 "flush"
#define SE_WIPE2 "wipe"
#define SE_REM_SERVICE_FILES "rem_data"
#define SE_SERVICE_PATH "service_path"
#define SE_INDEX_PATH "index_path"
#define SE_CACHE_PATH "cache_path"
#define SE_LOAD "load"
#define SE_SAVE "save"
#define SE_ADD "add"
#define SE_UPDATE "update"
#define SE_DEBUG "debug"
#define SE_REM "rem"
#define SE_SEARCH "search"
#define SE_PRINT_INDEX "index"
#define SE_PRINT_FILES "files"
#define SE_PRINT_WORDS "words"
#define SE_CLS1 "clear"
#define SE_CLS2 "cls"

const vString allowedExt = { ".txt" };

class SearchEngine
{
private:
	bool DEBUG = true;

	InvertedIndex __index;
	FileList __files;
	ui maxSERP;
	ui maxDocsPerWord;

	static const fs::path INDEX_FILE_NAME;
	static const fs::path CACHE_FILE_NAME;
	fs::path serviceInfoPath;
	fs::path cache_path;
	fs::path index_path;

	vPath SearchEngine::getFiles(const fs::path& path, bool recursively)
	{
		try {
			if (!fs::exists(path))
				Error("Path doesn`t exists.");

			vPath res;

			if (!fs::is_directory(path))
				return vPath({ path });

			auto lambda = [&res = res](fs::directory_entry entry)
			{
				if (!fs::is_directory(entry) &&
					find(allowedExt.begin(), allowedExt.end(), entry.path().extension()) != allowedExt.end())
				{
					res.push_back(entry.path());
				}
			};

			if (recursively)
				traverseDirectory<true>(path, lambda);
			else
				traverseDirectory<false>(path, lambda);
			return res;
		}
		catch (std::exception e)
		{
			ColorMassege(e.what(), 0x0c);
			return vPath();
		}
	}

	void SerializeCache() const
	{
		std::ofstream ofs(cache_path.string());

		for (auto& doc : __files) {
			ofs << doc.first << std::endl;
			ofs << doc.second << std::endl;
		}

		ofs.close();
	}


	void SerializeIndex() const
	{
		std::ofstream ofs(index_path);

		for (auto& word : __index)
		{
			ofs << word.first << std::endl;
			ofs << word.second.size() << std::endl;

			for (auto& doc : word.second)
			{
				ofs << doc.first << std::endl;
				ofs << doc.second << std::endl;
			}
		}

		ofs.close();
	}

	void deSerializeIndex()
	{
		std::string in, word;
		size_t cnt, freq;
		std::ifstream ifs;

		ifs.open(index_path);

		while (std::getline(ifs, word))
		{
			WordInDoc wid;
			ifs >> cnt;

			for (size_t i = 0; i < cnt; i++)
			{
				ifs >> freq; ifs.ignore();
				std::getline(ifs, in);
				wid.insert(std::make_pair(freq, in));
			}
			__index.insert(std::make_pair(word, wid));
		}
		ifs.close();
	}

	void deSerializeCache()
	{
		FileInfo fi;
		std::string file;
		std::ifstream ifs;
		ifs.open(cache_path);
		while (std::getline(ifs, file))
		{
			ifs >> fi; ifs.ignore();
			__files.insert(std::make_pair(fs::path(file), fi));
		}
		ifs.close();
	}
public:
	void showHelpMsg();

	SearchEngine(const fs::path& path = SE_SERVICE_ROOT_PATH)
		: maxSERP(5), maxDocsPerWord(5)
	{
		serviceInfoPath = path;
		setServiceFilesNames();
	}

	void setServiceFilesNames()
	{
		cache_path = serviceInfoPath / SearchEngine::CACHE_FILE_NAME;
		index_path = serviceInfoPath / SearchEngine::INDEX_FILE_NAME;
	}

	void clearServiceData()
	{
		fs::remove(cache_path);
		fs::remove(index_path);
		if (fs::is_empty(serviceInfoPath))
		{
			fs::remove(serviceInfoPath);
		}
	}

	fs::path servicePath() const
	{
		return serviceInfoPath;
	}

	WordInDoc search(vString words) const
	{
		WordInDoc res;
		DocRank docRank;

		for (auto& word : words)
		{
			InvertedIndex::const_iterator it;
			if (!Lang::ProcessWord(word) ||
				(it = __index.find(word)) == __index.end())
			{
				continue;
			}

			ui total = 0;
			for (auto it2 = it->second.begin();
				it2 != it->second.end() && total < maxDocsPerWord; ++it2)
			{
				// add separate func to process cacl of weight of word
				docRank[it2->second] += it2->first;
				++total;
			}
		}

		for (auto doc : docRank)
		{
			res.insert(std::make_pair(doc.second, doc.first));
		}
		return res;
	}

	void printSERP(const WordInDoc& serp) const
	{
		ui cnt = 0;
		for (auto doc : serp)
		{
			ColorMassege(std::to_string(doc.first) + " " + doc.second.string(), 0x0a);
			if (++cnt >= maxSERP)
				break;
		}
	}

	fs::path getIndexPath() const
	{
		return index_path;
	}

	fs::path getCachePath() const
	{
		return cache_path;
	}

	void wipe()
	{
		__index.clear();
		__files.clear();
	}

	bool dataIntegrity() const
	{
		return fs::exists(index_path) && fs::exists(cache_path);
	}

	void load()
	{
		try {
			if (!dataIntegrity())
				Error("Index doesn`t exists");
			wipe();
			deSerializeCache();
			deSerializeIndex();
			update();
		}
		catch (std::exception e)
		{
			ColorMassege(e.what(), 0x0c);
		}
	}

	void removeFromIndex(const fs::path& rootPath, bool recursively = false)
	{
		vPath paths = getFiles(rootPath, recursively);
		for (const auto& path : paths)
		{
			auto tmp_it = __files.find(path);
			if (tmp_it == __files.end())
				continue;

			__files.erase(tmp_it);

			bool wordEmpty = false;

			for (auto word = __index.begin(); word != __index.end(); )
			{
				for (auto it = word->second.begin(); it != word->second.end(); )
				{
					if (it->second == path.string())
					{
						if (word->second.size() == 1)
						{
							__index.erase(word++);
							wordEmpty = true;
						}
						else {
							word->second.erase(it++);
						}
						break;
					}
					else
					{
						++it;
					}
				}
				if (!wordEmpty)
					++word;
				else
					wordEmpty = false;
			}
		}
	}

	void printIndex() const
	{
		for (const auto& file : __files)
		{
			std::cout << file.first << std::endl;
			for (const auto& word : __index)
			{
				bool find = false;
				for (const auto& entity : word.second)
				{
					if (entity.second == file.first)
					{
						find = true;
						break;
					}
				}
				if (find) {
					std::cout << word.first << ' ';
				}
				else {}
			}
			std::cout << std::endl;
		}
	}

	void printFiles() const
	{
		for (const auto& file : __files)
		{
			std::cout << file.first << std::endl;
		}
		std::cout << "Total: " << __files.size() << std::endl;
	}

	void printWords() const
	{
		for (const auto& word : __index)
		{
			std::cout << word.first << std::endl;
		}
		std::cout << "Total: " << __index.size() << std::endl;
	}

	ui wordsLength() const
	{
		return __index.size();
	}

	bool outdatedFile(const FileList::const_iterator it) const
	{
		using cl = fs::file_time_type::clock;
		return cl::to_time_t(fs::last_write_time(it->first)) != cl::to_time_t(it->second.last_modified)
			|| hashfile(it->first.string().c_str()) != it->second.hash;
	}

	void update()
	{
		for (FileList::const_iterator it = __files.begin(); it != __files.end(); )
		{
			FileList::const_iterator it2 = std::next(it);
			std::string action;
			fs::path path = it->first;
			WORD color;
			if (outdatedFile(it))
			{
				action = "Updated";
				color = 0x0b;
				updateFileInIndex(path);
			}
			else {
				action = "Not modified";
				color = 0x08;
			}
			if (DEBUG) {
				std::cout << path.string() << ' ';
				ColorMassege(action, color);
			}
			it = it2;
		}
	}

	void updateFileInIndex(const fs::path& path)
	{
		removeFromIndex(path);
		addFileToIndex(path);
	}

	void readFile(const fs::path& path)
	{
		std::ifstream ifs;
		try {
			ifs.open(path.string().c_str());

			WordFreq wf;
			std::string line;
			while (!ifs.eof())
			{
				std::getline(ifs, line);
				FormatStr(line);
				vString words = split(line);
				for (auto& word : words)
				{
					if (!Lang::ProcessWord(word)) continue;
					wf[word] = wf.find(word) == wf.end() ? 1 : ++wf[word];
				}
			}
			for (auto& word : wf)
			{
				if (__index.find(word.first) != __index.end())
				{
					__index[word.first].insert(std::make_pair(word.second, path));
				}
				else {
					WordInDoc wid;
					wid.insert(std::make_pair(word.second, path));
					__index.insert(std::make_pair(word.first, wid));
				}
			}
		}
		catch (std::exception e)
		{
			ColorMassege(e.what(), 0x0c);
		}
	}

	bool isDebug() const {
		return DEBUG;
	}

	void setDebugFlag(bool flag) {
		DEBUG = flag;
	}

	void Serialize() const {
		fs::create_directory(serviceInfoPath);
		SerializeCache();
		SerializeIndex();
	}

	void addToIndex(const fs::path& rootPath, bool recursively)
	{
		vPath paths = getFiles(rootPath, recursively);
		for (const auto& path : paths)
		{
			auto it = __files.find(path);
			std::string action;
			WORD color;
			if (it != __files.end() && outdatedFile(it)) { // Update
				action = "Updated";
				color = 0x0b;
				updateFileInIndex(path);
			}
			else if (it == __files.end()) { // Add
				action = "Added";
				color = 0x0a;
				addFileToIndex(path);
			}
			else {
				color = 0x0d;
				action = "Exists";
			}
			if (DEBUG) {
				std::cout << path.string() << ' ';
				ColorMassege(action, color);
			}
		}
	}

	void addFileToIndex(const fs::path& path)
	{
		__files.insert(std::make_pair(path, FileInfo(path)));
		readFile(path);
	}

	InvertedIndex getIndex() const {
		return __index;
	}

	FileList getFiles() const {
		return __files;
	}
};

const fs::path SearchEngine::INDEX_FILE_NAME = "index.se";
const fs::path SearchEngine::CACHE_FILE_NAME = "cache.se";

void SearchEngine::showHelpMsg()
{
	std::string help =
		SE_REM " [-r] <dir1> <dir2> ... <dirN>\r\n"
		SE_ADD " [-r] <dir1> <dir2> ... <dirN>\r\n"
		SE_SEARCH " <w1> <w2> ... <wN>\r\n"
		SE_LOAD " -- load index from cache\r\n"
		SE_SAVE " -- save index\r\n"
		SE_UPDATE " -- update loaded index\r\n"
		SE_WIPE1 " OR " SE_WIPE2 " -- wipe loaded index\r\n"
		SE_REM_SERVICE_FILES " -- delete service files\r\n"
		"params[--show <max_docs>][--<param1> <value1> ... --<paramN> <valueN>]\r\n"
		"\r\n"
		SE_DEBUG " -- get debug flag\r\n"
		SE_CLS1 " OR " SE_CLS2 " -- cls screen\r\n"
		SE_EXIT1 " OR " SE_EXIT2 " -- close program";

	if (this->isDebug()) {
		help +=
			"\r\n\r\n"
			SE_INDEX_PATH " -- get index path\r\n"
			SE_CACHE_PATH " -- get cache path\r\n"
			SE_SERVICE_PATH " -- get service path\r\n"
			SE_PRINT_INDEX " -- print index\r\n"
			SE_PRINT_FILES " -- print files\r\n"
			SE_PRINT_WORDS " -- print words";			
	}
	std::cout << help << std::endl;
}