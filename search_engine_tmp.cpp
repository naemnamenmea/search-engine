std::vector<std::string> getDirectoryFiles(const std::string& path) {
	std::vector<std::string> files;

	for (auto& p : fs::directory_iterator(path))
	{
		std::string str = p.path().string();

		if (str.length() >= 4 && str.substr(str.length() - 4) == ".txt")
		{
			files.push_back(str);
		}
	}
	return files;
}

std::string SearchEngine::getPathFromInput(std::string input)
{
	std::string pathKey = "--path";
	std::string path = "";
	size_t pos = input.find(pathKey);
	if (pos != std::string::npos) {
		std::string cuted = input.substr(pos + pathKey.length() + 1);

		std::smatch matches;
		//std::regex regex(R"("(\w:\\)?([\w\s0-9_]\\)*[\w]+\.txt")");
		std::regex regex(R"(^"c:\\file.txt"$)");
		if (std::regex_search(cuted, matches, regex)) {
			size_t non_space_pos = cuted.find_first_not_of(' ');
			cuted = cuted.substr(non_space_pos);
			std::cout << non_space_pos << std::endl;
			std::cout << cuted << std::endl;
			std::cout << matches[0] << std::endl;
			std::cout << cuted.find(matches[0]) << std::endl;
			if (cuted.find(matches[0]) == 0) {
				path = matches[0];
			}
		}
	}
	/*
	должно работать с кавычками и без них
	проверить на работу с пробелами
	проверить если впереди будет мусор

	add_file --path ""
	add_file --path "c:\file.txt"
	add_file --path "Test_data\\doc_quick_search\\docs 1251\\2.txt"

	/^(([a-zA-Z]:)|(\{2}w+)$?)(\(w[w ]*))+.(txt|TXT)$/si

	Регулярное выражение совпадет
	c:file.txt | c:foldersub folderfile.txt | networkfolderfile.txt
	Регулярное выражение не совпадет
	C: | C:file.xls | folder.txt
	*/

	return path;
}