#include "avl.h"
avl::avl() { //default and only constructor for the tree
	head = nullptr;
}
avl::node::node(int idIn, std::string nameIn) { //node constructor
	id = idIn;
	name = nameIn;
	left = nullptr;
	right = nullptr;
	height = 1; //gets updated after being inserted
}

void avl::updateHeight(avl::node* n) { //used to simplify code
	n->height = max(height(n->left), height(n->right))+1;
}
int avl::height(avl::node* n) { //made to avoid nullptr errors
	return (n == nullptr) ? (0) : (n->height);
}
int avl::max(int a, int b) { //simplifies code
	return (a >= b) ? (a) : (b);
}
bool avl::isBadText(std::string in) {
	if (in.find_first_not_of("ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz ") == std::string::npos) {
		return false;
	}
	return true;
}
void avl::insert(int idIn, std::string nameIn) { //only inserts the head or calls the recursive insert function
	if (isBadText(nameIn)) {
		std::cout << "unsuccessful\n";
		return;
	}
	if (head == nullptr) { //when inserting the first node make it the head
		head = new node(idIn, nameIn);
		std::cout << "successful\n";
	}
	else {
		bool success = false; //taken in as a reference so we know if the recursive method works
		insertRecurse(idIn, nameIn, head, success);
		if (!success) {
			std::cout << "unsuccessful\n";
		}
		else {
			head = balance(head);
			updateHeight(head);
		}
	}

}
void avl::insertRecurse(int idIn, std::string nameIn, node* n, bool& b) { //recursively inserts new nodes
	if (idIn < n->id) { //check the left
		if (n->left != nullptr) {
			insertRecurse(idIn, nameIn, n->left, b); //recurse
		}
		else {
			n->left = new node(idIn, nameIn); //insert
			std::cout << "successful\n";
			b = true;
		}
		n->left = balance(n->left);  //check to see if balancing is needed.
	}
	else if(idIn > n->id) { //check the right
		if (n->right != nullptr) {
			insertRecurse(idIn, nameIn, n->right, b); //recurse
		}
		else {
			n->right = new node(idIn, nameIn); //insert
			std::cout << "successful\n";
			b = true;
		}
		n->right = balance(n->right);  //check to see if balancing is needed.
	}
	updateHeight(n); //update height
	//update(n); //update heights
}
void avl::insertNode(node* in) { //this section is just used to re-add nodes who's parents were deleted.
	insertNodeRecurse(in, head);
}
void avl::insertNodeRecurse(node* in, node* n) {
	if (in->id < n->id) { //basically just simplified insert()
		if (n->left != nullptr) {
			insertNodeRecurse(in, n->left);
		}
		else {
			n->left = in;
		}
	}
	else {
		if (n->right != nullptr) {
			insertNodeRecurse(in, n->right);
		}
		else {
			n->right = in;
		}
	}
	updateHeight(n);
}


void avl::remove(int idIn) {
	if (head == nullptr) {
		std::cout << "unsuccessful\n";
		return;
	}
	if (idIn == head->id) { //special case if removing the head
		node* old = head;
		if (old->right != nullptr && old->left != nullptr) {//check left and right to see if there are children to worry abt
			head = old->right;
			insertNodeRecurse(old->left, head); //add in the old children so they don't get deleted too
		}
		else if (old->right != nullptr) { 
			head = old->right;
		}
		else if (old->left != nullptr) {
			head = old->left;
		}
		else {
			head = nullptr;
		}
		delete old; //remove old node from memory
		updateHeight(head);
		std::cout << "successful\n";
		return;
	}
	removeRecurse(idIn, head); //call the recursive removal algorithm
}
void avl::removeRecurse(int idIn, node* n) {
	if (idIn < n->id) { //check left
		if (n->left != nullptr) { //have to look at the node's children since a node needs to be deleted via it's parent
			if (idIn == n->left->id) {
				node* old = n->left;
				if (old->right != nullptr && old->left != nullptr) {
					n->left = old->right;
					insertNodeRecurse(old->left, n->left);
				}
				else if (old->right != nullptr) {
					n->left = old->right; //replace nodes in order of succession.
				}
				else if (old->left != nullptr) {
					n->left = old->left;
				}
				else {
					n->left = nullptr;
				}
				delete old;
				std::cout << "successful\n";
			}
			else if (n->left != nullptr) { //if it wasn't the left node, recurse
				removeRecurse(idIn, n->left);
			}
			else {
				std::cout << "unsuccessful\n";
			}
		}
		else {
			std::cout << "unsuccessful\n";
		}
	}
	else if (n->right != nullptr) { //check right
		if (idIn == n->right->id) {
			node* old = n->right;
			if (old->right != nullptr && old->left != nullptr) {
				n->right = old->right;
				insertNodeRecurse(old->left, n->right);
			}
			else if (old->right != nullptr) {
				n->right = old->right;
			}
			else if (old->left != nullptr) {
				n->right = old->left;
			}
			else {
				n->right = nullptr;
			}
			delete old;
			std::cout << "successful\n";
		}
		else if (n->right != nullptr) { //if it wasn't the right node, recurse
			removeRecurse(idIn, n->right); 
		}
		else {
			std::cout << "unsuccessful\n";
		}
	}
	else {
		std::cout << "unsuccessful\n";
	}
	updateHeight(n);

}
void avl::removeInorder(int n) {
	if (head == nullptr) {
		std::cout << "unsuccessful\n";
		return;
	}
	int count = 0;
	bool b = false;
	removeInorderRecurse(n, head, count, b); //call the recursive method
	if (!b) {
		std::cout << "unsuccessful\n";
	}

}

bool avl::removeInorderRecurse(int m, node* n, int& count, bool& b) {
	if (count > m) { //look left, then root, then right
		return false;
	}	if (n->left != nullptr) {
		if (removeInorderRecurse(m, n->left, count, b)) {
			node* old = n->left;
			if (old->right != nullptr && old->left != nullptr) {
				n->left = old->right;
				insertNodeRecurse(old->left, n->left); //replace removed nodes
			}
			else if (old->right != nullptr) {
				n->left = old->right;
			}
			else if (old->left != nullptr) {
				n->left = old->left;
			}
			else {
				n->left = nullptr;
			}
			delete old;
			std::cout << "successful\n";
			b = true;
			updateHeight(n);
			return false;
		}
	}
	count++; // increment count

	if (count-1 == m) {
		if (n == head) { //if head
			remove(head->id); //tell the main removal method to remove the head, this is O(1) since it doesn't recurse;
			b = true;
			return false;
		}
		return true;

	}

	if (n->right != nullptr) {
		if (removeInorderRecurse(m, n->right, count, b)) {
			node* old = n->right;
			if (old->right != nullptr && old->left != nullptr) {
				n->right = old->right;
				insertNodeRecurse(old->left, n->left);
			}
			else if (old->right != nullptr) {
				n->right = old->right;
			}
			else if (old->left != nullptr) {
				n->right = old->left;
			}
			else {
				n->right = nullptr;
			}
			delete old;
			std::cout << "successful\n";
			b = true;
			updateHeight(n);
			return false;
		}
	}
	return false;
}
//citing stepik, used it to help figure the rotations
avl::node* avl::turnLeft(node* n)
{
	node* rc = n->right;
	n->right = rc->left;
	rc->left = n;
	n->height = max(height(n->left), height(n->right)) + 1;
	rc->height = max(height(rc->left), height(rc->right)) + 1;
	return rc;
}
avl::node* avl::turnRight(node* n)
{
	//std::cout << "LLcase\n";
	node* lc = n->left;
	n->left = lc->right;
	lc->right = n;
	n->height = max(height(n->left), height(n->right)) + 1;
	lc->height = max(height(lc->left), height(lc->right)) + 1;
	return lc;
}
avl::node* avl::turnLeftRight(node* n) {
	//std::cout << "LRcase\n";
	n->left = turnLeft(n->left);
	return turnRight(n);
}
avl::node* avl::turnRightLeft(node* n) {
	//std::cout << "RLcase\n";
	n->right = turnRight(n->right);
	return turnLeft(n);
}

int avl::checkBalance(node* n) {
	if (n == nullptr) { //avoids nullptr errors
		return 0; 
	}
	return (height(n->left) - height(n->right)); //balance factor calculation
}
avl::node* avl::balance(node* n) {
	int bal = checkBalance(n); //get balance factor
	if (bal <= 1 && bal >= -1) {
		return n;
	}

	if (bal < -1) {
		if (checkBalance(n->right) < 0) {
			//Right-Right Case
			return turnLeft(n);
		}
		else {
			//Right Left Case
			return turnRightLeft(n);
		}
	}

	if (bal > 1) {
		if (checkBalance(n->left) > 0) {
			//Left-Left Case
			return turnRight(n);
		}
		else {
			//Left-Right Case
			return turnLeftRight(n);
		}
	}
	return n;

}

void avl::searchName(std::string nameIn) {
	std::vector<int> printQ; //taken in as a reference, solves the problem of duplicate names
	searchNameRecurse(nameIn, head, printQ);
	if (printQ.size() == 0) {
		std::cout << "unsuccessful\n";
	}
	else {
		for (int i = 0; i < printQ.size(); i++) { //print out the names, which are stored in a vector
			std::cout << printQ.at(i) << "\n";
		}
	}

}
void avl::searchNameRecurse(std::string nameIn, node* n, std::vector<int>& printQ) { //recursively goes through entire list, has to since there could be duplicate names.
	if (n == nullptr) {
		return;
	}
	if (n->name == nameIn) {
		printQ.push_back(n->id);
	}
	if (n->left != nullptr) {
		searchNameRecurse(nameIn, n->left, printQ);
	}
	if (n->right != nullptr) {
		searchNameRecurse(nameIn, n->right, printQ);
	}
	return;
}
void avl::searchID(int idIn) {
	std::string nameOut = "";
	searchIDRecurse(idIn, head, nameOut);
	if (nameOut == "") {
		std::cout << "unsuccessful\n";
	}
	else {
		std::cout << nameOut << "\n"; //ids are unique so no need for list
	}

}
bool avl::searchIDRecurse(int idIn, node* n, std::string& nameOut) {
	if (n == nullptr) {
		return false;
	}
	if (n->id == idIn) {
		nameOut = n->name; 
		return true;
	}
	bool b = false;
	if (idIn < n->id) { //speedy implementation, no need to search entire list thanks to rules of binary tree
		b = searchIDRecurse(idIn, n->left, nameOut);
	}
	else {
		b = searchIDRecurse(idIn, n->right, nameOut);

	}
	return b;
}
avl::node* avl::searchIDpoint(int idIn) { //these next two are just for debugging
	std::string nameOut = "";
	node* n = searchIDRecursePoint(idIn, head, nameOut);
	return n;

}
avl::node* avl::searchIDRecursePoint(int idIn, node* n, std::string& nameOut) {
	if (n == nullptr) {
		return nullptr;
	}
	if (n->id == idIn) {
		nameOut = n->name;
		return n;
	}
	bool a = false;
	bool b = false;
	node* n1 = searchIDRecursePoint(idIn, n->left, nameOut);
	node* n2 = searchIDRecursePoint(idIn, n->right, nameOut);
	if (n->left != nullptr&& n1!=nullptr) a = idIn == n1->id;
	if (a) return n->left;
	if (n->right != nullptr&& n2 != nullptr) b = idIn == n2->id;
	if (b) return n->right;
	return nullptr;
}

void avl::printInorder() {
	if (head == nullptr) {
		std::cout << "\n";
		return;
	}
	std::vector<std::string> printq; //list of everything we need to print.
	printInorderRecurse(head, printq);
	for (int i = 0; i < printq.size(); i++) {
		std::cout << printq.at(i);
		if (i < printq.size() - 1) { //adds a comma unless its the end of the list
			std::cout << ", ";
		}
		else {
			std::cout << "\n";
		}
	}
}
void avl::printInorderRecurse(node* n, std::vector<std::string>& printQ) {
	if (n->left != nullptr) printInorderRecurse(n->left, printQ); //check left, root, right
	printQ.push_back(n->name);
	if (n->right != nullptr) printInorderRecurse(n->right, printQ);
	return;
}
void avl::printPreorder() { //Preorder and Postorder are the Inorder code but slightly changed.
	if (head == nullptr) {
		std::cout << "\n";
		return;
	}
	std::vector<std::string> printq;
	printPreorderRecurse(head, printq);
	for (int i = 0; i < printq.size(); i++) {
		std::cout << printq.at(i);
		if (i < printq.size() - 1) {
			std::cout << ", ";
		}
		else {
			std::cout << "\n";
		}
	}
}
void avl::printPreorderRecurse(node* n, std::vector<std::string>& printQ) {
	printQ.push_back(n->name);
	if (n->left != nullptr) printPreorderRecurse(n->left, printQ);
	if (n->right != nullptr) printPreorderRecurse(n->right, printQ);
	return;
}
void avl::printPostorder() {
	if (head == nullptr) {
		std::cout << "\n";
		return;
	}
	std::vector<std::string> printq;
	printPostorderRecurse(head, printq);
	for (int i = 0; i < printq.size(); i++) {
		std::cout << printq.at(i);
		if (i < printq.size() - 1) {
			std::cout << ", ";
		}
		else {
			std::cout << "\n";
		}
	}
}
void avl::printPostorderRecurse(node* n, std::vector<std::string>& printQ) {
	if (n->left != nullptr) printPostorderRecurse(n->left, printQ);
	if (n->right != nullptr) printPostorderRecurse(n->right, printQ);
	printQ.push_back(n->name);
	return;
}
void avl::printLevelCount() {
	if (head == nullptr) { //height is 0 when list is empty.
		std::cout << "0\n";
		return;
	}
	std::cout << height(head) << "\n"; //height is 1 on the bottom floor and counted going up, so the head height is the number of levels. Much easier to implement this way.
}
