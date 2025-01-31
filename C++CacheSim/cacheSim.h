#pragma once
#include <string>
#include <iostream>
#include <vector>
using namespace std;

class cacheSim
{
public:

	char type;
	int lines;
	int bytes;
	int blocksize;
	int offieldlen;
	char repType;
	int globalCount;
	string BLANK;
	int hits;
	int total;
	cacheSim(int bytes, int blocksize, char type, string hex, char rep) { //constructor, take in parameters such as the type of cache
		lines = bytes / blocksize;
		repType = rep; //replacement type, l for least recently used, f for first in first out
		this->blocksize = blocksize;
		this->bytes = bytes;
		this->type = type;
		globalCount = 0;
		offieldlen = log2(blocksize);
		string bin = inputGood(hex);
		BLANK = inputGood("0x00000000");
		for (size_t i = 0; i < lines; i++)
		{
			data.push_back(new line(bin, *this, hex));
		}
	}
	string hitrate() { //calculate the hit rate of the cache (% of rqs served from cache)
		string s = to_string((hits * 1.0 / (total * 1.0)) * 100);
		s+="%";
		return s;
	}
	class line {

	public:
		line(string bin, cacheSim& cache, string hex) { //line subclass, stores binary and hex info
			this->hex = hex;
			binary = bin;
			counter = 0;

		}
		virtual void print() {
			cout << binary << endl;
		}
		string binary;
		string hex;

		string tag;
		int counter;
	};

	vector<line*> data;

	virtual line* getLine(string address) = 0; //virtual methods defined by child classes
	virtual void addLine(line* in) = 0;

	void print() {
		cout << "print:" << endl;
		for (auto i : data) {
			i->print();
		}
		cout << "done" << endl;
	}

	void doCounter(line* in) { //update counter, used to track what gets replaced. counter is handled differently for LRU and FIFO.
		if (repType == 'l') {
			in->counter++;
		}
		else {
			globalCount++;
			in->counter = globalCount;

		}
	}
	string inputGood(string in) { //create a binary representation of a string
		string bin = "";
		in = in.substr(2, in.length());
		//cout << in << "in"<<endl;
		for (char i : in) {
			switch (i) {
			case '0':
				bin += "0000";
				break;

			case '1':
				bin += "0001";
				break;
			case '2':
				bin += "0010";
				break;
			case '3':
				bin += "0011";
				break;
			case '4':
				bin += "0100";
				break;
			case '5':
				bin += "0101";
				break;
			case '6':
				bin += "0110";
				break;
			case '7':
				bin += "0111";
				break;
			case '8':
				bin += "1000";
				break;
			case '9':
				bin += "1001";
				break;
			case 'a':
			case 'A':
				bin += "1010";
				break;
			case 'b':
			case 'B':
				bin += "1011";
				break;
			case 'c':
			case 'C':
				bin += "1100";
				break;
			case 'd':
			case 'D':
				bin += "1101";
				break;
			case 'e':
			case 'E':
				bin += "1110";
				break;
			case 'f':
			case 'F':
				bin += "1111";
				break;
			default:
				break;
			}
		}
		return bin;
	}
};

class fullCache : public cacheSim { //fully associative cache
public:
	fullCache(int bytes, int blocksize, char type, string hex, char repType) : cacheSim(bytes, blocksize, type, hex, repType) {
	}
	class fullLine : public line {
	public:
		fullLine(string bin, fullCache& cache, string hex) :line(bin, cache, hex) {
			tag = (bin.substr(0, bin.length() - cache.offieldlen));
		}
		void print() {
			cout << tag <<" " <<binary<<" " << hex << endl;
		}
	};
	line* getLine(string inHex) { //take in a hex line, check if it exists in data, add it if it doesnt
		total++;
		string hex = inHex.substr(2, inHex.length());

		string inBin = inputGood(inHex); //convert to binary
		fullLine* toAdd = new fullLine(inBin, *this, hex);
		//cout << seek << endl;
		for (line* i : data) {
			if (toAdd->tag == i->tag) { //check if tag already exists
				if (repType == 'l') { //if LRU update counter
					globalCount++;
					(i)->counter = globalCount;
				}
				hits++; //one more hit
				return i;
			}
		}
		addLine(toAdd); //line does not exist, add it
		return NULL;
	}
	void addLine(line* toAdd) {

		for (int i = 0; i < data.size(); i++) {
			if (data[i]->binary == BLANK) { //if data has a blank space
				delete data.at(i);
				data[i] = toAdd; //replace it with the line we're adding

				globalCount++;
				data.at(i)->counter = globalCount;

				return;
			}

		}
		doReplace(toAdd); //otherwise, replace something based on the replacement type
	}
	void doReplace(line* in) { //replace data with new data
		int min = INT_MAX;
		int minIdx = -1;
		for (int idx = 0; idx < data.size(); idx++) {
			if (data.at(idx)->counter < min) { //lowest counter means either least recently used or first in depending on which rep type is in use
				min = data.at(idx)->counter;
				minIdx = idx; //index of data to be replaced
			}
		}
		delete data.at(minIdx);
		data.at(minIdx) = in; //out with the old data, in with the new
		globalCount++;
		data.at(minIdx)->counter = globalCount;

	}
};

class setCache : public cacheSim { //set associative cache
public:
	int setNum;
	int setLen;
	setCache(int bytes, int blocksize, char type, string hex, int setNum, char repType) : cacheSim(bytes, blocksize, type, hex, repType) {
		setLen = log2(lines / setNum); //length of a set is determined by how many lines are available and whether the cache is 2-way associative, 4-way, etc...
		this->setNum = setNum;
	}
	int setIdx(int idx) {
		return (idx / setNum)*setNum;
	}
	class setLine : public line {
		public:
			setLine(string bin, setCache& cache, string hex) :line(bin, cache, hex) {

				set = bin.substr(bin.length() - cache.offieldlen - cache.setLen, cache.setLen); //which set this line goes in
				tag = (bin.substr(0, bin.length() - cache.offieldlen - cache.setLen));

			}
			void print() {
				cout << tag << " " << set <<" "<< binary <<" "<<hex<< endl;
			}
			string set;
		};
	line* getLine(string inHex) { //take in a hex line, check if it exists in data, add it if it doesnt
		total++;
		string hex = inHex.substr(2, inHex.length());

		string inBin = inputGood(inHex);
		setLine* toAdd = new setLine(inBin, *this, hex);


		int seek = (stoi(toAdd->set, 0, 2)*setNum) % data.size();
		int idx = setIdx(seek); //find the idx of the set the data woukd be
		for (int i = (idx); i < idx + setNum && i < data.size(); i++) { //look through a set

			if (toAdd->tag==data.at(i)->tag) {
				if (repType == 'l') {
					globalCount++;
					(data.at(i))->counter = globalCount;
				}
				hits++;
				return data.at(i);
			}
		}
		addLine(toAdd); //add it in if it doesnt exist
		return NULL;
	}
	void addLine(line* toAdd) {

		int seek = (stoi(dynamic_cast<setLine*>(toAdd)->set, 0, 2) * setNum) % data.size();
		int idx = setIdx(seek); //find index of set we want to add the line to
		for (int i = (idx); i < idx + setNum && i < data.size(); i++) { //look through the set
			if (data.at(i)->binary == BLANK) { //if set has an empty slot, add the new line in its place
				delete data.at(i);
				data.at(i) = toAdd;
				return;
			}
		}
		replace(toAdd, seek);
		//replace stuff otherwise based on rep strategy
	}
	void replace(line* in, int idx) {
		int min = INT_MAX;
		int minIdx = -1;
		idx = setIdx(idx);
		for (int i = idx; i < idx + setNum && i < data.size(); i++) { //find data in set most replaceable as determined by rep strategy
			if (data.at(i)->counter < min) {
				min = data.at(i)->counter;
				minIdx = i;
			}
		}

		delete data.at(minIdx);
		data.at(minIdx) = in;
		globalCount++;
		data.at(minIdx)->counter = globalCount; //replace old data with new

	}
};

class directCache : public cacheSim { //direct-mapped cache
public:
	int lfLen;
	directCache(int bytes, int blocksize, char type, string bin, char repType) : cacheSim(bytes, blocksize, type, bin, repType) {
		lfLen = log2(lines); //length of the non-tag part of the line, the part that determines what cache line a line goes to, more lines means a larger one of these is needed to uniquely identify all the cache slots.
	}
	class directLine : public line {
	public:
		directLine(string bin, directCache& cache, string hex) :line(bin, cache, hex) {
			lf = (bin.substr(bin.length() - cache.offieldlen - cache.lfLen, cache.lfLen));
			tag = (bin.substr(0, bin.length() - cache.offieldlen - cache.lfLen));

		}
		void print() {
			cout << tag << " " << lf<<" "<< binary << " " << hex << endl;
		}
		string lf;
	};
	line* getLine(string inHex) { //check if a line exists in the cache, add it if it doesn't

		total++;

		string hex = inHex.substr(2, inHex.length());

		string inBin = inputGood(inHex);
		directLine* toAdd = new directLine(inBin, *this, hex);


		int seek = stoi(toAdd->lf, 0, 2) % data.size();

		if (data.at(seek)->tag == toAdd->tag) { //is the line at the spot it would be if it was in the cache
			doCounter(data.at(seek));

			hits++;
			return data.at(seek); //if it is, we found it, that's a hit, return

		}
		addLine(toAdd); //add the line in where it should be.
		return NULL;

	}
	void addLine(line* toAdd) { //no replacement strategy used since this cache is directly associative.
		int tempLf = stoi(dynamic_cast<directLine*>(toAdd)->lf, 0, 2)%data.size(); //find cache slot directly mapped to memory address of line

		delete data.at(tempLf);
		data.at(tempLf) = toAdd; //replace the data there with the new line

	}
};

