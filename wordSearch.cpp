//wordSearch.cpp

#include "wordSearch.h"
#include <cgicc/CgiDefs.h> 
#include <cgicc/Cgicc.h> 
#include <cgicc/HTTPHTMLHeader.h> 
#include <cgicc/HTMLClasses.h>

using namespace cgicc;

wordSearch::wordSearch(){
	hashBase = 101;
	for(int count = 97; count < 123; count++){
                alphabet.push_back(char(count));
        }
	importDict();
}

void wordSearch::importDict(){
	std::ifstream in("twl.txt");
	std::string w;
	while(std::getline(in,w)){
		if(w.back() == '\n' || w.back() == '\r'){
			w.pop_back();
		}
		dictionary[w[0]].push_back(w);
	}
	in.close();
}

int wordSearch::compHash(std::string substr){
	int length = substr.length();
	int hashVal = 0;

	int count = length - 1;
	for(int i = 0; i < length; i++){
		hashVal += int(pow(hashBase, count)) * int(substr[i]);
		count--;
	}
	return hashVal;
}

void wordSearch::permute(std::string substr, int index, std::vector<char> available, std::string result, std::vector<std::string> &resultArr){
	if(index == substr.length()){
		resultArr.push_back(result);
		return;
	}
	if(substr[index] != ' '){
		result.push_back(substr[index]);
		wordSearch::permute(substr,index+1,available,result,resultArr);
	}
	else{
		for(int i = 0; i < available.size(); i++){
			std::string res = result;
			res.push_back(available[i]);
			std::vector<char> newAvail = available;
			newAvail.erase(newAvail.begin()+i);
			wordSearch::permute(substr,index+1,newAvail,res,resultArr);
		}
	}
}

std::unordered_map<int,std::vector<std::string>> wordSearch::search(std::string str){
	int length = str.length();
	std::unordered_map<int,std::vector<std::string>> targetStr; //hash maps to word
	std::unordered_map<int,std::vector<std::string>> foundStr;  //wordlength maps to word

	int spaceFront = 0;
	int spaceBack = 0;
	std::string substr = str;
	while(substr.front() == ' '){
		substr = substr.substr(1,length);
		spaceFront++;
	}
	while(substr.back() == ' '){
		substr.pop_back();
		spaceBack++;
	}
	
	length = substr.length();

	//Add permutations to targetStr (substrings to be searched for)
	if(substr.find(' ') != -1){
		std::vector<std::string> perm;
		std::string str;
		wordSearch::permute(substr, 0, avail, str, perm);
		for(int i = 0; i < perm.size(); i++){
			int h = wordSearch::compHash(perm[i]);
			targetStr[h].push_back(perm[i]);
		}
	}
	else{
		int h = wordSearch::compHash(substr);
		targetStr[h] = std::vector<std::string>(1,substr);
	}

	//Search dictionary for substrings in targetStr
	std::set<char> availLetters(avail.begin(),avail.end());
	for(int i = 0; i < substr.length(); i++){
		if(substr[i] == '?'){
			availLetters = std::set<char>(alphabet.begin(),alphabet.end());
			break;
		}
		else{
			availLetters.insert(substr[i]);
		}
	}	

	for(std::set<char>::iterator i = availLetters.begin(); i != availLetters.end(); i++){
		char letter = *i;
		for(int j = 0; j < dictionary[letter].size(); j++){
			std::string word = dictionary[letter][j];
			int wordLength = word.length();

			std::vector<char> available = avail;
			int numWild = 0;
			for(int k = 0; k < substr.length(); k++){
				if(substr[k] == '?'){
					numWild++;
				}	
				else{
					available.push_back(substr[k]);
				}
			}
			for(int k = 0; k < numWild; k++){
				available.push_back('?');
			}

			if(substr.empty()){
				for(int x = 0; x < wordLength; x++){
					bool valid = false;
					for(std::vector<char>::iterator y = available.begin(); y != available.end(); y++){
						if(*y == word[x] || *y == '?'){
							available.erase(y);
							valid = true;
							break;
						}
					}
					if(!valid){
						break;
					}
					if(valid && x == wordLength - 1){
						foundStr[wordLength].push_back(word);
					}
				}
			}
			else if (wordLength >= length){
				bool found = false;
				std::string s = word.substr(0,length);
				int h = wordSearch::compHash(s);
				for(int x = 0; x < wordLength; x++){
					bool valid = false;
					for(std::vector<char>::iterator y = available.begin(); y != available.end(); y++){
						if(*y == word[x] || *y == '?'){
							available.erase(y);
							valid = true;
							break;
						}
					}
					if(!valid){
						break;
					}
					if(x == 0 && ((spaceFront == 0 && wordLength-x-length <= spaceBack) || (spaceFront == 0 && spaceBack == 0))){
						if(targetStr.find(h) != targetStr.end()){
							for(int it = 0; it < targetStr[h].size(); it++){
								if(s.compare(targetStr[h][it]) == 0){
									found = true;
									break;
								}
							}
						}
					}
					else if (x <= wordLength - length){
						if((spaceFront == 0 && spaceBack == 0) || (spaceFront == 0 && wordLength-x-length <= spaceBack) || (x <= spaceFront && spaceBack == 0)){
							s = word.substr(x,length);
							h = hashBase * ( h - (int(pow(hashBase, length - 1)) * int(word[x-1]))) + int(s[length-1]);
							if(targetStr.find(h) != targetStr.end()){
            	                for(int it = 0; it < targetStr[h].size(); it++){
                	                if(s.compare(targetStr[h][it]) == 0){
                    	                found = true;
                        	            break;
                        			}
                   	 			}
							}
						}
					}
					if (x == wordLength - 1 && found){
						foundStr[wordLength].push_back(word);
					}
				}
			}
		}
	}
	return foundStr;
}

bool wordSearch::userInput(){
	Cgicc formData;
	
	wordSearch::printSite();
	
	std::string input;
	form_iterator fi = formData.getElement("avail");
	if(!fi->isEmpty() && fi!= (*formData).end()){
	  input = formData("avail");
	  cout << "<p class=\"query-text\">Available letters: " << input << "</p>" << endl;
	}
	else{
	  cout << "No letters available." << endl;
	}
	for(int i = 0; i < input.size(); i++){
		avail.push_back(input[i]);
	}

	std::string substr;
	substr = formData("substr");
	cout << "<p class=\"query-text\">Substring: " << substr << "</p>" << endl;
	while(substr.back() == '\n' || substr.back() == '\r'){
		substr = substr.substr(0,substr.size()-1);
	}
	std::unordered_map<int,std::vector<std::string>> result = search(substr);

	printOutput(result);
	
	return true;
}

void wordSearch::clearPrev(){
	avail.clear();
}

void wordSearch::printOutput(std::unordered_map<int,std::vector<std::string>> found){
	for(int i = 11; i > 0; i--){
		if(found.find(i) != found.end()){
			cout << "<div>" << endl;
	        cout << "<p class=\"info-text\">Words of length: " << i << "</p>" << endl;
    	    cout << "<p class=\"result-text\">";
        	for(int j = 0; j < found[i].size(); j++){
 	           cout << found[i][j] << " ";
    	    }
        	cout << "</p>" << endl;
			cout << "</div>" << endl;

		}
	}
	cout << "</body>" << endl;
	cout << "</html>" << endl;
}

void wordSearch::printSite(){
	cout << "Content-data: text/html\n\n";
	
	std::ifstream in("search.html");
	std::string w;
        while(std::getline(in,w)){
		cout << w << endl;
        }
        in.close();
}

int main(){
	wordSearch w;
	bool success = w.userInput();
}
