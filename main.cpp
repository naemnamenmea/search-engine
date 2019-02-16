#include <iomanip>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <iostream>
#include <windows.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "index.hpp"


#ifndef WIN32
//#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

int main(const int argc, const char* argv[])
{
	SetConsoleCP(1251); // установка кодовой страницы win-cp 1251 в поток ввода
	SetConsoleOutputCP(1251); // установка кодовой страницы win-cp 1251 в поток вывода

	SearchEngine se;
	std::string input;

	while (std::cout << "> " && std::getline(std::cin, input)) {
		vString args = split(input);

		try {
			if (args.size() == 0)
				continue;

			if (args[0] == SE_EXIT1 || args[0] == SE_EXIT2)
			{
				return 0;
			}
			else if (args[0] == SE_HELP)
			{
				SearchEngine::showHelpMsg();
			}
			else if (args[0] == SE_WIPE1 || args[0] == SE_WIPE2)
			{
				se.wipe();
			}
			else if (args[0] == SE_REM_SERVICE_FILES) {
				se.clearServiceData();
			}
			else if (args[0] == SE_SERVICE_PATH) {
				std::cout << se.servicePath() << std::endl;
			}
			else if (args[0] == SE_INDEX_PATH) {
				std::cout << se.getIndexPath() << std::endl;
			}
			else if (args[0] == SE_CACHE_PATH) {
				std::cout << se.getCachePath() << std::endl;
			}
			else if (args[0] == SE_LOAD)
			{
				se.load();
			}
			else if (args[0] == SE_SAVE) {
				se.Serialize();
			}
			else if (args[0] == SE_ADD)
			{
				if (args.size() == 2 && args[1] == "-r" || args.size() == 1) {
					ColorMassege("Nothing to add.", 0x0c);
					continue;
				}

				bool recursively = args[1] == "-r";
				auto it = ++args.begin();
				if (recursively)
					++it;

				while (it != args.end()) {
					se.addToIndex(fs::path(*it), recursively);
					it++;
				}

			}
			else if (args[0] == SE_UPDATE) {
				se.update();
			}
			else if (args[0] == SE_DEBUG) {
				if (args.size() == 1) {
					std::cout << "DEBUG = " << se.isDebug() << std::endl;
				}
				else if (args.size() == 2 && (args[1] == "0" || args[1] == "1")) {
					se.setDebugFlag(std::stoi(args[1]));
				}
				else {
					ColorMassege("Nothing to delete.", 0x0c);
				}
			}
			else if (args[0] == SE_REM) {
				if (args.size() == 2 && args[1] == "-r" || args.size() == 1) {
					ColorMassege("Nothing to delete.", 0x0c);
					continue;
				}

				bool recursively = args[1] == "-r";
				auto it = ++args.begin();
				if (recursively)
					++it;

				while (it != args.end()) {
					se.removeFromIndex(fs::path(*it));
					it++;
				}
			}
			else if (args[0] == SE_SEARCH)
			{
				if (args.size() < 2) {
					ColorMassege("Your query is empty.", 0x0c);
				}
				else {
					vString keys;
					std::move(std::next(args.begin()), args.end(), std::back_inserter(keys));
					auto res = se.search(keys);
					se.printSERP(res);
				}
			}
			else if (args[0] == SE_PRINT_INDEX) {
				se.printIndex();
			}
			else if (args[0] == SE_PRINT_FILES) {
				se.printFiles();
			}
			else if (args[0] == SE_PRINT_WORDS) {
				se.printWords();
			}
			else if (args[0] == SE_CLS1 || args[0] == SE_CLS2)
			{
				system("cls");
				continue;
			}
			else
			{
				ColorMassege("Wrong command. Type \"help\" to get command list...", 0x0c);
				continue;
			}
			ColorMassege("Done", 0x0a);
		}
		catch (std::exception e)
		{
			ColorMassege(e.what(), 0x0c);
		}
	}

	return 0;
}