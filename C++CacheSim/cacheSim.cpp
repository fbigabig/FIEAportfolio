// cachesim.cpp : This file contains the 'main' function. Program execution begins and ends there.
// Aaron Gill 4/16/2023

#include "cacheSim.h"
#include <fstream>
void readLine(cacheSim* cache, fstream& f) {
	string temp;
	f >> temp;
	f >> temp;
	//cout << "getting: " << temp << endl;
	cache->getLine(temp);
	f >> temp;
}
void test(cacheSim* cache) {
	fstream f;
	f.open("trace.txt");

	while (true) {
		readLine(cache, f);
		//cache->print();

		if (f.eof()) {
			break;
		}
	}
	//cache->print();

}

void test2(cacheSim* cache) {
	cache->getLine("0x1fffff78");
	cache->print();
	for (auto i : cache->data) {
		cout << i->counter << endl;
	}
	cache->getLine("0x2effff78");
	cache->print();
	for (auto i : cache->data) {
		cout << i->counter << endl;
	}
	cache->getLine("0x3fbfff78");

	cache->print();
	for (auto i : cache->data) {
		cout << i->counter << endl;
	}
	cache->getLine("0x4fafff78");

	cache->print();
	for (auto i : cache->data) {
		cout << i->counter << endl;
	}
	cache->getLine("0x1fffff78");

	cache->getLine("0x5fafff78");
	cache->print();
	for (auto i : cache->data) {
		cout << i->counter << endl;
	}
	cache->getLine("0x6fffff78");
	cache->print();
	for (auto i : cache->data) {
		cout << i->counter << endl;
	}
	cache->getLine("0x7fdfff78");
	cache->print();
	for (auto i : cache->data) {
		cout << i->counter << endl;
	}
	cache->getLine("0x1fffff78");

	cache->getLine("0x86ffff78");
	cache->print();
	for (auto i : cache->data) {
		cout << i->counter << endl;
	}
	cache->getLine("0x9fffff78");

	cache->print();
	for (auto i : cache->data) {
		cout << i->counter << endl;
	}
}
int main()
{
	fstream p;
	p.open("param.txt");
	int size, blocksize;
	char type, policy;
	for (int i = 0; i < 1; i++) {
	p >> size;
	p >> blocksize;
	p >> type;
	p >> policy;
	cout << "size: " << size << "  blocksize" << blocksize << endl;
	vector<cacheSim*> caches;

	//this part gets tweaked to test different cache types
	caches.push_back(new directCache(size, blocksize, 'd', "0x00000000", 'l'));
	//caches.push_back(new setCache(size, blocksize, 's', "0x00000000", 1, 'l'));

	caches.push_back(new setCache(size, blocksize, 's', "0x00000000", 2, 'l'));
	//caches.push_back(new setCache(size, blocksize, 's', "0x00000000", 4, 'l'));
	//caches.push_back(new setCache(size, blocksize, 's', "0x00000000", 8, 'l'));
	caches.push_back(new fullCache(size, blocksize, 'f', "0x00000000", 'l'));
	//caches.push_back(new setCache(size, blocksize, 's', "0x00000000", 2, 'f'));
	//caches.push_back(new setCache(size, blocksize, 's', "0x00000000", 4, 'f'));
	//caches.push_back(new setCache(size, blocksize, 's', "0x00000000", 8, 'f'));
	//caches.push_back(new fullCache(size, blocksize, 'f', "0x00000000", 'f'));*/

	//cache1->print();
	//cache2->print();
	for (auto cache : caches) {
		test(cache);
		//cache->print();
		cout << cache->hitrate() /* << " hits: " << cache->hits << " misses" << cache->total - cache->hits*/ << endl;
	}
	//cout << caches.at(0)->data.at(0)->binary << endl;
	cout << endl;
}
	//cout<<caches[0]->lines<<" "<<caches[0]->offieldlen << " " << (dynamic_cast<setCache*>(caches[0]))->setLen << " " <<caches[0]->total<< endl;
	//cache3->print();
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
