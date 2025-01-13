#include "MemoryManager.h"
#include <cmath>
#include <fstream>
#include <fcntl.h>
#include <string.h>
#include <iterator>
#include <algorithm>
#include <unistd.h>
int worstFit(int sizeInWords, void *list) { //fix later
    uint16_t* holeList = static_cast<uint16_t*>(list);
    int index = -1;
    int max = -1;
    //cout<<"worstfit"<<endl;
    uint16_t holeListlength = holeList[0];
    //cout<<"holeListlength: "<<holeListlength<<" "<<holeList[0]<<endl;
    for(int i = 1; i<holeListlength*2; i+=2){
        //cout<<"test"<<endl;
        //cout<<holeList[i]<<" "<<holeList[i+1]<<endl;
        if(holeList[i+1] >= sizeInWords && holeList[i+1] > max){
            index = holeList[i];
            max = holeList[i+1];
        }
    }
    //cout<<"done"<<endl; 
    return index;
}

int bestFit(int sizeInWords, void *list){
    uint16_t* holeList = static_cast<uint16_t*>(list);
    
    int index = -1;
    int min = 65537;
    //cout<<"bestFit"<<endl;
    uint16_t holeListlength = holeList[0];
    //cout<<"holeListlength: "<<holeListlength<<" "<<holeList[0]<<endl;
    for(int i = 1; i<holeListlength*2; i+=2){
        //cout<<"test"<<endl;
        //cout<<holeList[i]<<" "<<holeList[i+1]<<endl;
        if(holeList[i+1] >= sizeInWords && holeList[i+1] < min){
            index = holeList[i];
            min = holeList[i+1];
        }
    }
    //cout<<"done"<<endl; 
    return index;
}
void MemoryManager::initialize(size_t sizeInWords){
    //holes = vector<block*>();
    //blocks = map<int, block*>;
    if(sizeInWords>65536) {
        return;
    }
    if(allocated!=nullptr) {
        shutdown();
    }
    shutdownCalled = false;
    //holeList = &holes;
    bytesTotal = sizeInWords * wordSize;
    //memory = malloc(bytesTotal);
    allocated = new char[bytesTotal];
    this->sizeInWords = static_cast<int>(sizeInWords);
    holes.emplace(0, new block(0, sizeInWords, allocated));
    //allocated = new vector<block*>();
    wordsLeft = this->sizeInWords;
}
void MemoryManager::shutdown(){
    shutdownCalled = true;
    for(auto i: blocks){
        delete(i.second);
    }
    blocks.clear();
    for(auto i: holes){
        delete(i.second);
    }
    //cout<<"shutdown"<<endl;
    holes.clear();
    //::free(holeList);
    if(allocated!=nullptr){
    delete[] (allocated);
    allocated=nullptr;
}
    //blocks.shrink_to_fit();
}
void* MemoryManager::allocate(size_t sizeInBytes){
    if(sizeInBytes==0) {
        return nullptr;
    }
    //cout<<"allocate"<<endl;
    int allocateWordSize = ceil(static_cast<double>(sizeInBytes*(1.0 / wordSize)));
    if(allocateWordSize > wordsLeft){
        //cout<<"sizeInWords > wordsLeft"<<endl;
        return nullptr;
    }
    holeList = getList();
    int offset = allocator(allocateWordSize, holeList);
    if(offset == -1){
        //cout<<"offset == -1"<<endl;
        return nullptr;
        
    }
    delete[] (uint16_t*)holeList;
    //cout<<"offset: "<<offset<<endl; 

    
    block* hole = holes.at(offset);
    void* temp = holes.at(offset)->ptr;
    block* allocatedBlock = new block(hole->start, allocateWordSize, temp);
    
    blocks.emplace(hole->start, allocatedBlock);

    if(hole->size > allocateWordSize){
        holes.emplace(hole->start+allocateWordSize, new block(hole->start + allocateWordSize, hole->size - allocateWordSize, (char*)allocated+(hole->start + allocateWordSize)*wordSize));
    }
    delete holes.at(offset);
    holes.erase(offset);
    wordsLeft -= allocateWordSize;
    return allocatedBlock->ptr;
}
void MemoryManager::free(void *address){ //breaks here
    
    //cout<<"free"<<endl;
    bool freed=false;
    map<int,block*>::iterator hole;
    for(map<int,block*>::iterator it=blocks.begin(); it!=blocks.end(); ++it){

        if(it->second->ptr == address){
            //free(address);
            wordsLeft += it->second->size;
            hole = (holes.emplace(it->first, new block(it->first, it->second->size,address))).first;
            delete blocks.at(it->first);
            blocks.erase(it->first);

            freed=true;
            break;
        }
    }
    if(freed) {
        
        
        map<int,block*>::iterator nxt = next(hole);
        if(hole!=holes.begin()) {
            map<int,block*>::iterator b4 = prev(hole);
            if(b4->first+b4->second->size == hole->first) {
                b4->second->size += hole->second->size;
                delete holes.at(hole->first);
                holes.erase(hole->first);
                hole = b4;
            }
        }
        if(nxt!=holes.end()) {
            if(hole->first+hole->second->size == nxt->first) {
                hole->second->size += nxt->second->size;
                delete holes.at(nxt->first);
                holes.erase(nxt->first);
            }
        }
    }
    //cout<<"done"<<endl;
}
void MemoryManager::setAllocator(function<int(int, void *)> allocator){
    this->allocator = allocator;
}
int MemoryManager::dumpMemoryMap(char *filename){
    //cout<<"dumpMemoryMap"<<endl;
    if(holes.size()==0) {
        //cout<<"holes.size()==0"<<endl;
        return -1;
    }
    string s="";
    //cout<<holes.size()<<endl;
    for (auto i:holes) {
        s+="["+to_string(i.second->start)+", "+to_string(i.second->size)+"] - ";
    }
    int file = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0777);
    if(file==-1) {
        return -1;
    }
    s=s.substr(0,s.length()-3);
    const char *cstr = s.c_str();
    ssize_t bytes_written = write(file, cstr, strlen(cstr));
    if(bytes_written==-1) {
        return -1;
    }
    close(file);
    //cout<<"s: "<<s<<endl;
    return 0;
}
void* MemoryManager::getList(){
    //cout<<"list"<<"holeslistsize"<<holes.size()<<" new size total thingy"<<(1+holes.size()*2)<<endl;
    //int* list = new int[1+holes.size()*2];
    if(allocated==nullptr) {
        return nullptr;
    }
    uint16_t* list =  new uint16_t[1+holes.size()*2];
    list[0] = holes.size();
    int j =0;
    for(auto i:holes){
        list[1+j*2] = i.first;
        list[2+j*2] = i.second->size;
        //cout<<list[1+j*2]<<" "<<list[2+j*2]<<endl; 
        j++;
    }
    ////cout<<list->size()<<endl;
    return list;
}
void* MemoryManager::getBitmap(){
    //cout<<"bitmap"<<endl;
    int bytes = ceil(sizeInWords/8.0);
    string bitmap[bytes];
    uint8_t* final = new uint8_t[2+bytes];
    //cout<<bytes+2<<endl;
    final[1] = (bytes)>>8 & ((1 <<8)-1);
    final[0] = (bytes) & ((1 <<8)-1);
    fill_n(bitmap, bytes, "");
    for(int i=0; i<bytes; i++){
        //cout<<bitmap[i]<<endl;
    }
    for(int i=0; i<blocks.begin()->first; i++){
        //cout<<0;
        bitmap[i/8] +="0";
    }
    map<int,block*>::iterator i = blocks.begin();
    while(true)
    {
        int start  = i->first;
        int end = start + i->second->size;
        for(int j = start; j<end; j++){
            //cout<<1;
            bitmap[j/8] +="1";
        }
        i++;
        if(i==blocks.end()) {
            //cout<<"end"<<endl;
            for(int j = end; j<bytes*8; j++){
                //cout<<0;
                bitmap[j/8] +="0";
            }
            break;
        }
        int nxt = i->first;
        
        for(int j = end; j<nxt; j++){
            ////cout<<"j: "<<j<<endl;
            //cout<<0;
            bitmap[j/8] +="0";
        }
    }
    for(int i=0; i<bytes; i++){
        //cout<<"bitmap of "<<i<<": "<<bitmap[i]<<endl;
        
    }
    for(int i=0; i<bytes; i++){
        reverse(bitmap[i].begin(), bitmap[i].end());
        final[2+i] = stoi(bitmap[i], nullptr, 2);
    }
    return final;
}
unsigned MemoryManager::getWordSize(){
    return wordSize;
}
void* MemoryManager::getMemoryStart(){
    return allocated;
}
unsigned MemoryManager::getMemoryLimit(){
    return bytesTotal;
}