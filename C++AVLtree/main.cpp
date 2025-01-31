#include <iostream>
#include <sstream>
//#include <fstream>
#include "avl.h"

//implemented here to avoid code reuse in main
std::string removeQuotes(std::string s) {
	std::string s1;
	for (char c : s) {
		if (c != '"') {
			s1 += c;
		}
	}
	return s1;
}
int main(){
	//setup variables
	avl tree;
	int commands;

	//get number of commands
	std::cin >> commands;
	//get to the next line
	std::string tmp;

	std::getline(std::cin, tmp); //get rid of useless input

	for (int i = 0; i < commands; i++) //loop to get thru every command
	{
		//get input
		std::string input = "";
		std::getline(std::cin, input);
		std::stringstream stream(input); //stream through the command as a line of text
		std::string command;
		stream >> command;

		if (command == "insert") {
			//insertion case
			std::string name;
			int id;
			getline(stream, tmp,'"'); //get rid of useless input
			getline(stream, name, '"');

			name = removeQuotes(name);
			std::string tmpid;
			stream >> tmpid;
			if (tmpid.size() != 8) {
				std::cout << "unsuccessful\n";
			}
			else {
				id = std::stoi(tmpid);
				tree.insert(id, name);
			}
		}
		else if (command == "remove") {
			//removal case
			int id;
			stream >> id;
			tree.remove(id);
		}
		else if (command == "removeInorder") {
			//remove INORDER case
			int num;
			stream >> num;
			tree.removeInorder(num);
		}
		else if (command == "search") {
			//both search cases in this section
			std::string toFind;
			getline(stream, tmp, ' '); //get rid of useless imput
			getline(stream, toFind);

			bool isNumber = true;
			//determine if input is a number or string
			for (char c : toFind) {
				if (!isdigit(c)) {
					isNumber = false;
					break;
				}
			}
			if (isNumber) { //implementation of search ID
				tree.searchID(std::stoi(toFind));
			}
			else { //implementation of search Name
				toFind = removeQuotes(toFind);
				tree.searchName(toFind);
			}
		}
		//printing cases
		else if (command == "printInorder") {
			tree.printInorder();
		}
		else if (command == "printPreorder") {
			tree.printPreorder();
		}
		else if (command == "printPostorder") {
			tree.printPostorder();
		}
		else if (command == "printLevelCount") {
			tree.printLevelCount();
		}
		else {
			std::cout << "unsuccessful\n";
		}
	}




	return 0;
}

