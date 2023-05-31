/*
The author is --> https://gist.github.com/nitrix/34196ff0c93fdfb01d51
*/

#pragma once

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdint>
#include <iomanip>

std::string hashfile(const char *filename)
{
	std::ifstream fp(filename);
	std::stringstream ss;

	// Unable to hash file, return an empty hash.
	if (!fp.is_open()) {
		return "";
	}

	// Hashing
	uint32_t magic = 5381;
	char c;
	while (fp.get(c)) {
		magic = ((magic << 5) + magic) + c; // magic * 33 + c
	}

	ss << std::hex << std::setw(8) << std::setfill('0') << magic;
	return ss.str();
}