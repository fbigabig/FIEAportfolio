#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <map>
class avl {
private:
	struct node { //struct since all of this gets modified by the class, basic tree node, contains a name and id
		int id;
		std::string name;
		node(int idIn, std::string nameIn);
		node* left;
		node* right;
		int height;
	};

	node* head; //only actual variable of the class

	//methods used by a variety of methods to simplify code and balance the AVL
	node* turnLeft(node* n);
	node* turnRight(node* n);
	node* turnLeftRight(node* n);
	node* turnRightLeft(node* n);
	int height(node* n);
	void updateHeight(node* n);
	int max(int a, int b);
	//void update(node* n);
	node* balance(node* n);
	int checkBalance(node* n);

	//the recursive methods called by the public methods
	node* searchIDRecursePoint(int idIn, node* n, std::string& nameOut);
	void removeRecurse(int idIn, node* n);
	bool removeInorderRecurse(int m, node* n, int& count, bool& b);
	void insertRecurse(int idIn, std::string nameIn, node* n, bool& b);
	void insertNodeRecurse(node* in, node* n);
	void printInorderRecurse(node* n, std::vector<std::string>& printQ);
	void printPreorderRecurse(node* n, std::vector<std::string>& printQ);
	void printPostorderRecurse(node* n, std::vector<std::string>& printQ);
	void searchNameRecurse(std::string nameIn, node* n, std::vector<int>& printQ);
	bool searchIDRecurse(int idIn, node* n, std::string& nameOut);

public:

	avl(); //default and only constructor, sets head to nullptr

	//public methods with recursive private methods
	void insert(int idIn, std::string nameIn);
	void insertNode(node* in);
	void remove(int idIn);
	void removeInorder(int n);
	void printInorder();
	void printPreorder();
	void printPostorder();
	void searchName(std::string nameIn);
	void searchID(int idIn);
	node* searchIDpoint(int idIn);
	bool isBadText(std::string in);

	void printLevelCount(); //except for this one, this is O(1) since it just gets the head's height.







};
