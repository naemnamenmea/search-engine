/*
������������� ������ ��������� ������� �� C++
����� --> http://manunich.blogspot.com/2016/01/c-cpp.html
*/

#pragma once

#include <vector>
#include <string>
#include <regex>
#include "common.hpp"

namespace Russian
{
	bool essential(const std::string&);
}
void FormateWord(std::string&);

namespace Lang {
	bool ProcessWord(std::string& word) {
		FormateWord(word);
		return Russian::essential(word);
	}
}

namespace Russian {
	namespace Stemmer {
		class Porter {
		private:
			static const std::string EMPTY;
			static const std::string S1;
			static const std::string S13;
			static const std::string SN;
			static const std::string const1;
			static const std::regex PERFECTIVEGROUND;
			static const std::regex REFLEXIVE;
			static const std::regex ADJECTIVE;
			static const std::regex PARTICIPLE;
			static const std::regex VERB;
			static const std::regex NOUN;
			static const std::regex I;
			static const std::regex P;
			static const std::regex NN;
			static const std::regex DERIVATIONAL;
			static const std::regex DER;
			static const std::regex SUPERLATIVE;

		public:
			Porter();
			std::string stem(std::string s);
		};

		const std::string Porter::const1 = "����������";
		const std::string Porter::EMPTY = "";
		const std::string Porter::S1 = "$1";
		const std::string Porter::S13 = "$1$3";
		const std::string Porter::SN = "�";
		const std::regex Porter::PERFECTIVEGROUND = std::regex("(��|����|������|��|����|������|���|�����)$");
		const std::regex Porter::REFLEXIVE = std::regex("(��|��)$");
		const std::regex Porter::ADJECTIVE = std::regex("(��|��|��|��|���|���|��|��|��|��|��|��|��|��|���|���|���|���|��|��|��|��|��|��|��|��)$");
		const std::regex Porter::PARTICIPLE = std::regex("(.*)(���|���|���)$|([��])(��|��|��|��|�)$");
		const std::regex Porter::VERB = std::regex("(.*)(���|���|���|����|����|���|���|���|��|��|��|��|��|��|��|���|���|���|��|���|���|��|��|���|���|���|���|��|�)$|([��])(��|��|���|���|��|�|�|��|�|��|��|��|��|��|��|���|���)$");
		const std::regex Porter::NOUN = std::regex("(�|��|��|��|��|�|����|���|���|��|��|�|���|��|��|��|�|���|��|���|��|��|��|�|�|��|���|��|�|�|��|��|�|��|��|�)$");
		const std::regex Porter::I = std::regex("�$");
		const std::regex Porter::P = std::regex("�$");
		const std::regex Porter::NN = std::regex("��$");
		const std::regex Porter::DERIVATIONAL = std::regex(".*[^���������]+[���������].*����?$");
		const std::regex Porter::DER = std::regex("����?$");
		const std::regex Porter::SUPERLATIVE = std::regex("(����|���)$");

		Porter::Porter() {
			setlocale(0, "");
		}
		std::string Porter::stem(std::string s) {
			transform(s.begin(), s.end(), s.begin(), ::toupper);
			replace(s.begin(), s.end(), '�', '�');
			int k = 0;
			size_t pos = s.find_first_of(const1, 0);
			if (pos != std::string::npos) {
				std::string pre = s.substr(0, pos + 1);
				std::string rv = s.substr(pos + 1);
				std::string temp = regex_replace(rv, PERFECTIVEGROUND, EMPTY);
				if (rv.size() != temp.size()) {
					rv = temp;
				}
				else {
					rv = regex_replace(rv, REFLEXIVE, EMPTY);
					temp = regex_replace(rv, ADJECTIVE, EMPTY);
					if (rv.size() != temp.size()) {
						rv = temp;
						rv = regex_replace(rv, PARTICIPLE, S13);
					}
					else {
						temp = regex_replace(rv, VERB, S13);
						if (rv.size() != temp.size()) {
							rv = temp;
						}
						else {
							rv = regex_replace(temp, NOUN, EMPTY);
						}
					}
				}
				rv = regex_replace(rv, I, EMPTY);
				if (regex_match(rv, DERIVATIONAL)) {
					rv = regex_replace(rv, DER, EMPTY);
				}
				temp = regex_replace(rv, P, EMPTY);
				if (temp.length() != rv.length()) {
					rv = temp;
				}
				else {
					rv = regex_replace(rv, SUPERLATIVE, EMPTY);
					rv = regex_replace(rv, NN, SN);
				}
				s = pre + rv;
			}
			transform(s.begin(), s.end(), s.begin(), tolower);
			return s;
		}
	}

	bool essential(const std::string& str) {
		return !(str.length() < 3);
	}
}