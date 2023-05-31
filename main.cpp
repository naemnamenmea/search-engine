#include <iomanip>
#include <algorithm>
#include <string>
#include <cstdlib>

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
//#include <unistd.h>
#endif

#ifdef WIN32
#define stat _stat
#endif

#include "search_engine.hpp"
#include "se_aux.hpp"

int main(const int argc, const char* argv[])
{
	SearchEngine se("se_data");
	std::string input;
	while (std::cout << "> " && std::getline(std::cin, input)) {
		vString args = split(input);

		if (args.size() == 0) continue;

		if (args[0] == "exit")
		{
			return 0;
		}
		else if (args[0] == "help")
		{
			SearchEngine::showHelpMsg();
		}
		else if (args[0] == "build_index")
		{
			std::string path = getValueByKey("--path", args);
			std::string index = getValueByKey("--index", args);
			bool expand = checkKey("--expand", args);
			bool serialize = checkKey("--serialize", args);

			se.buildIndex(path, expand);
		}
		else if (args[0] == "search")
		{
			vString keys = getValuesByKey("--query", args);

			auto res = se.search(keys);
			for (auto& it = res.rbegin(); it != res.rend(); ++it) {
				std::cout << std::setw(10) << it->first << " : " << it->second << std::endl;
			}
		}
		else if (args[0] == "clear")
		{
			system("cls");
		}
		else
		{
			std::cout << "Wrong command" << std::endl;
		}
	}

	return 0;
}