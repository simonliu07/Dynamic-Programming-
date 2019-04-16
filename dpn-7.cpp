/* Simon Liu Dynamic Programming mini-project 
//
//Purpose: 	Decode a file that had a compression failure, which causes all the white space among words in
//		text file to disapper. We are given a dictionary and the failed file as input, and we output
//		all the possible combinations of what the original file might be, with list of the frequency
//		list. Done through a Trie Tree for dictionary and used Dynamic Programming to find all 
//		possible solutions.
*/

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include <sstream>

using namespace std;
const int SIZE = 53;			//A ~ Z, ' , a ~ z
const int ALLOCATE_SIZE = 400000;	//set memory size to save the punctuation
int DATANUM = 0;
const double DICTIONARY_SIZE = 400000.0;

//The Trie Node each hold an array of child and frequency value
struct TrieNode{
	struct TrieNode *children[SIZE];
	double freq;
};

//Trie Node Constructor
struct TrieNode *getNode(void){
	struct TrieNode *pNode = new TrieNode;
	for(int i = 0; i < SIZE; ++i){
		pNode->children[i] = NULL;
	}
	pNode->freq = 0.0;
	return pNode;
}

//Purpose:	See whether the string have all char with ascii value of english character
bool english(string str){
	for(int i = 0; i < str.length(); ++i){
		if((int)str[i] > 127 || (int)str[i] < 39)
			return false;
	}
	return true;
}

//Purpose:	Insert word into the Trie Tree, when we add the last char of the word, set frequency value
void insert(struct TrieNode *root, string line, double frequency){
	struct TrieNode *pNext = root;
	for(int i = 0; i < line.length(); ++i){
		int index;
		if(isupper(line[i]))				//the if statement goes to the correct array location with its ascii value
			index = line[i] - 'A';
		else if(islower(line[i]))
			index = (line[i] - 'a') + 26;
		else if(line[i] == '\'')
			index = 52;
//		else 
//			return;
		if(!pNext->children[index])			//if there is no node in the child position, create Trie Node
			pNext->children[index] = getNode();
		pNext = pNext->children[index];
	}
	if(pNext->freq == 0.0){					//if this string was never created, set frequency value
		pNext->freq = frequency;
	}
}

//Purpose:	Find if the given string is a word in the Trie Tree
double search(struct TrieNode *root, string line){
	struct TrieNode *pNext = root;
	double frequency = 0;
	for(int i = 0; i < line.length(); ++i){
		int index;
		if(isupper(line[i]))			//if statement goes to the correct array location with its ascii value
			index = line[i] - 'A';
		else if(islower(line[i]))
			index = (line[i] - 'a') + 26;
		else if(line[i] == '\'')
			index = 52;
		else
			return frequency;		//if the char of the string isn't english => word not in Tree, return 0
		if(!pNext->children[index])		//if the char of the string isn't created => word not in Tree, return 0
			return frequency;
		pNext = pNext->children[index];
	}
	frequency = pNext->freq;			//word is found, return its frequency value
	return frequency;
}

//Purpose:	Break the string into component to find possible words, output into a file when done 
int wordBreak(string str, struct TrieNode *root, string result, int number,string *save, int counter, double sum, map<double,string> *data){
	int size = str.length();
	string prefix;
	int length;
	int location = 0;
	double frequency;
	if(size == 0){	//if we reach the end of the given string
		for(int i = 0; i < counter-2; i++){	//add in the punctuation back to the edited string
			prefix = save[i];		//go through the punctuation array of string
			length = prefix.size();
			if((prefix!=".")&&(prefix!="!")&&(prefix!="?")&&(prefix!=";")&&(prefix!="\n")){	//if its a string of a word
				for(int j = 0; j < length; j++){		//match all the char to find the next position for punctuation
					if(result[location] == ' '){
						--j;
					}
					else if(result[location] != prefix[j])	//if the char doesn't match 
						return 0;			//return 0 since it isn't a correct solution
					++location;
				}
			}
			else{	
				result.insert(location++,prefix);		//insert the punctuation into the edited string
			}
			
		}
		for(int i = 0; i < result.size(); ++i){				//find if there is a '\n' in the string
			if(result[i] == '\n'){					//if found, delete the space after it
				result.erase(i+2,1);		
				i++;
			}
		}
		for(int i = 0; i < result.size()-1; ++i){			//find whether the word after pucntuation is still a word in dictionary
			if((result[i] == '.') || (result[i] == '!') || (result[i] == '?') || (result[i] == ';') || (result[i] =='\n')){
				prefix = "";
				for(int j = i+1; j < result.size(); ++j){
					if(result[j] == ' ')
						break;
					else
						prefix += string(1,result[j]);
				}
				if(prefix.size() != 0){
					if(search(root,prefix) == 0)
						return 0;
				}
			}	
		}
		prefix = "solution_";
		stringstream ss;
		ss << DATANUM++;						//set number of output file
		prefix += ss.str();
		ofstream output((prefix + ".txt").c_str());			//open output file
		output << result;						//write the solution in
		output.close();
		prefix += ".txt";
		if(frequency != 0){
			std::map<double,string>::iterator p;
			while(1){						//make sure map doesn't replace file
				p = data->find(sum);
				if(p != data->end())
					sum = sum - 0.001;
				else
					break;
			}
				
		}
		data->insert(std::pair<double,string>(sum,prefix));		//save the output file name & frequency to map
		return 1;							//return 1 to symbolize a solution is found
		
	}
	int temp = 0;
	for(int i = 1; i <= size; ++i){			//go through the entire string
		prefix = str.substr(0, i);
		frequency = search(root, prefix);
		if(frequency != 0){			//if we found the word in the Trie Tree
			if(wordBreak(str.substr(i, size-i), root, result + prefix + " ", number-i, save, counter, sum+frequency, data) == 1)
				temp = 1;	//a solution is found
		}
	}
	return temp;
}

int main(int argc, char* argv[]){
	struct TrieNode *bank = getNode();
	ifstream fr;
	fr.open("dictionary3.txt");			//open dictionary3 
	string line;
	ifstream input;
	input.open(argv[1]);				//open corrupt file
	
	std::map<double, string> solutionMap;		//create map to save out file names & solution

	if(!fr){
		cout << "Error Opening Dictionary" << endl;
		return 0;
	}
	if(!input){
		cout << "Error Opening Input File" << endl;
		return 0;
	}
	
	double frequency = DICTIONARY_SIZE;
	while(getline(fr, line)){		//set the frequency number of words, largest means most frequent 
		if(line[0] != '#'){
			line.erase(line.size()-1);
			if(line.length() == 1){		//if the word is only 1 char, only import them if they're A, a or I
				if((line == "A") || (line == "a") || (line == "I")){
					insert(bank,line,frequency--);
				}
			}	
			else{			
				if(english(line)){	//if the word is english, import the word
					insert(bank, line, frequency--);
				}
			}
		}
	}
	line = "";
	string wholeThing;	//the string will contain the entire file without punctuations 
	string save[ALLOCATE_SIZE];
	int counter = 0;
	char c;
	while(input.get(c)){	
		if((c != '!') && (c != '.') && (c != '?') && (c != ';') && (c != ',') && (c != '\n')){//if the char isn't a punctuation
			line += string(1,c);
			wholeThing += string(1,c);
		}
		else if(c != ','){	//if we meet punctuation from the file, save the string, then save punctuation on the next line
			save[counter++] = line;
			line = string(1,c);
			save[counter++] = line;
			line = "";
		}
	}
	int complete = 0;
	if(wholeThing.size() != 0){	//do Dynamic Programming if there is something the corrupt file
		complete = wordBreak(wholeThing, bank, "", 0.0, save, counter, 0, &solutionMap);
	}
	if(complete == 0){//if no there is word that's not in the Trie Tree
		cout << "Word(s) not found in dictionary. No Output." << endl;
		return 0;
	}
		
	int temp = 1;
	std::map<double, string>::iterator p = solutionMap.end();
	--p;
	counter = solutionMap.size();
	cout << "List of file from most probable to least: " << endl;
	for(int i = 0; i < counter; ++i){	//output the list from highest frequency number to least
		cout << temp++ << ". " << "\t" << p->second << " " << endl;
		--p;
	}

	return 0;
}
