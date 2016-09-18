/* wordSearch header file
*/

#ifndef WORDSEARCH_H
#define WORDSEARCH_H

#include <unordered_map>
#include <vector>
#include <iostream>
#include <fstream>
#include <cmath>
#include <set>

using namespace std;

class wordSearch{
public:
	std::unordered_map<char, std::vector<std::string>> dictionary;
	std::vector<char> avail;
	int hashBase;
	std::vector<char> alphabet;
	
	wordSearch();
	void importDict();
	int compHash(std::string str);
	void permute(std::string substr, int index, 
		std::vector<char> available, std::string result, 
		std::vector<std::string> &resultArr);	
	std::unordered_map<int,std::vector<std::string>> search(std::string str);
	bool userInput();
	void clearPrev();
	void printOutput(std::unordered_map<int,std::vector<std::string>> found);	
	void printSite();
};

#endif
