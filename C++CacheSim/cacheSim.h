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
	cacheSim(int bytes, int blocksize, char type, string hex, char rep) {
		lines = bytes / blocksize;
		repType = rep;
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
	string hitrate() {
		string s = to_string((hits * 1.0 / (total * 1.0)) * 100);
		s+="%";
		return s;
	}
	class line {

	public:
		line(string bin, cacheSim& cache, string hex) {
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

	virtual line* getLine(string address) = 0;
	virtual void addLine(line* in) = 0;

	void print() {
		cout << "print:" << endl;
		for (auto i : data) {
			i->print();
		}
		cout << "done" << endl;
	}

	void doCounter(line* in) {
		if (repType == 'l') {
			in->counter++;
		}
		else {
			globalCount++;
			in->counter = globalCount;

		}
	}
	string inputGood(string in) {
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
			//cout << endl << "binip" << bin << " "<<bin.length()<<" "<<i<<endl;
		}
		//cout<<bin<<"   bin   "<<endl;
		return bin;
	}
};

class fullCache : public cacheSim {
public:
	fullCache(int bytes, int blocksize, char type, string hex, char repType) : cacheSim(bytes, blocksize, type, hex, repType) {
		//cout<<"test"<<endl;
	}
	class fullLine : public line {
	public:
		fullLine(string bin, fullCache& cache, string hex) :line(bin, cache, hex) {
			//cout<<"fullllinebin: "<<bin<<" "<<bin.length()<<endl;
			tag = (bin.substr(0, bin.length() - cache.offieldlen));
			//cout<<"tag:  "<<tag<<" "<<tag.length()<<endl;
		}
		void print() {
			cout << tag <<" " <<binary<<" " << hex << endl;
		}
	};
	line* getLine(string inHex) {
		total++;
		string hex = inHex.substr(2, inHex.length());

		string inBin = inputGood(inHex);
		//cout << "inbin: " << inBin << " " << inBin.length() << endl;
		fullLine* toAdd = new fullLine(inBin, *this, hex);
		//cout << seek << endl;
		for (line* i : data) {
			if (toAdd->tag == i->tag) {
				//cout << "found" << endl;
				if (repType == 'l') {
					globalCount++;
					(i)->counter = globalCount;
				}
				hits++;
				return i;
			}
		}
		addLine(toAdd);
		return NULL;
	}
	void addLine(line* toAdd) {
		//cout << "test" << endl;
		//cout << "add" << endl;

		//fullLine* toAdd = new fullLine(binIn, *this, hex);
		for (int i = 0; i < data.size(); i++) {
			if (data[i]->binary == BLANK) {
				//cout << "test2" << endl;
				delete data.at(i);
				data[i] = toAdd;

				globalCount++;
				data.at(i)->counter = globalCount;

				return;
			}

		}
		replace(toAdd);

		//cout << "test3" << endl;

		//replace stuff goes here
	}
	void replace(line* in) {
		//cout << "replace" << endl;
		int min = INT_MAX;
		int minIdx = -1;
		for (int idx = 0; idx < data.size(); idx++) {
			if (data.at(idx)->counter < min) {
				min = data.at(idx)->counter;
				minIdx = idx;
			}
		}
		delete data.at(minIdx);
		data.at(minIdx) = in;
		globalCount++;
		data.at(minIdx)->counter = globalCount;
		
	}
};

class setCache : public cacheSim {
public:
	int setNum;
	int setLen;
	setCache(int bytes, int blocksize, char type, string hex, int setNum, char repType) : cacheSim(bytes, blocksize, type, hex, repType) {
		setLen = log2(lines / setNum);
		this->setNum = setNum;
	}
	int setIdx(int idx) {
		return (idx / setNum)*setNum;
	}
	class setLine : public line {
		public:
			setLine(string bin, setCache& cache, string hex) :line(bin, cache, hex) {
				
				set = bin.substr(bin.length() - cache.offieldlen - cache.setLen, cache.setLen);
				tag = (bin.substr(0, bin.length() - cache.offieldlen - cache.setLen));
				//cout << bin << " " << tag << " " << set << " "<<cache.setLen<<cache.offieldlen<<endl;

			}
			void print() {
				cout << tag << " " << set <<" "<< binary <<" "<<hex<< endl;
			}
			string set;
		};
	line* getLine(string inHex) {
		total++;
		string hex = inHex.substr(2, inHex.length());

		string inBin = inputGood(inHex);
		setLine* toAdd = new setLine(inBin, *this, hex);

		//int seek = stoi(inBin.substr(0, inBin.length() - offieldlen - setLen), 0, 2);
		//cout << temp <<" temp "<< endl;
		int seek = (stoi(toAdd->set, 0, 2)*setNum) % data.size();
		int idx = setIdx(seek);
		for (int i = (idx); i < idx + setNum && i < data.size(); i++) {
			//cout<<"i"<<i<<" "<<data.size()<<" "<<data.at(i)->binary<<"seek "<<seek<<" "<<inBin<<" "<<data.at(i)->binary << endl;

			if (toAdd->tag==data.at(i)->tag) {
				//cout << "test";
				if (repType == 'l') {
					globalCount++;
					(data.at(i))->counter = globalCount;
				}
				hits++;
				return data.at(i);
			}
		}
		//cout << "test2";
		addLine(toAdd);
		return NULL;
	}
	void addLine(line* toAdd) {

		int seek = (stoi(dynamic_cast<setLine*>(toAdd)->set, 0, 2) * setNum) % data.size();
		int idx = setIdx(seek);
		for (int i = (idx); i < idx + setNum && i < data.size(); i++) {
			if (data.at(i)->binary == BLANK) {
				delete data.at(i);
				data.at(i) = toAdd;
				return;
			}
		}
		//cout << "help";
		replace(toAdd, seek);

		//replace stuff
	}
	void replace(line* in, int idx) {
		//cout << "here" << endl;
		int min = INT_MAX;
		int minIdx = -1;
		idx = setIdx(idx);
		for (int i = idx; i < idx + setNum && i < data.size(); i++) {
			if (data.at(i)->counter < min) {
				min = data.at(i)->counter;
				minIdx = i;
			}
		}
		//cout << minIdx << " " << idx << endl;
		int tempCounter = data.at(minIdx)->counter;
		delete data.at(minIdx);
		data.at(minIdx) = in;
		globalCount++;
		data.at(minIdx)->counter = globalCount;
		
	}
};

class directCache : public cacheSim {
public:
	int lfLen;
	directCache(int bytes, int blocksize, char type, string bin, char repType) : cacheSim(bytes, blocksize, type, bin, repType) {
		lfLen = log2(lines);
		//cout << lfLen << endl;
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
	line* getLine(string inHex) {

		total++;
		//cout << "Inhex: " << inHex << endl;
		string hex = inHex.substr(2, inHex.length());

		string inBin = inputGood(inHex);
		directLine* toAdd = new directLine(inBin, *this, hex);

		//cout << inBin<<" inbin " << endl;
		
		//cout << temp <<" temp "<< endl;
		int seek = stoi(toAdd->lf, 0, 2) % data.size();
		//cout<< seek << " seek " << " "<< seek % data.size()<< " "<<data.size()<<endl;
		if (data.at(seek)->tag == toAdd->tag) {
			//cout << "seek: " << seek << endl;
			doCounter(data.at(seek));
			//cout << "found :";
			//data.at(seek)->print();
			hits++;
			return data.at(seek);

		}
		//cout << "Test" << endl;
		addLine(toAdd);
		return NULL;

	}
	void addLine(line* toAdd) {
		int tempLf = stoi(dynamic_cast<directLine*>(toAdd)->lf, 0, 2)%data.size();
		//cout << tempLf << endl;

		delete data.at(tempLf);
		data.at(tempLf) = toAdd;
		//cout << "added: ";
		//data.at(tempLf)->print();
		//toAdd->print();
		//cout<<"oldvsnew"<<tempLf<<" "<<toAdd->lf<<" "<< stoi(toAdd->lf, 0, 2) % data.size()<<endl;
		//replace stuff
	}
};

