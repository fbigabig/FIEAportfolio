#include <functional>
#include <vector>
#include <map>
#include <iostream>
using namespace std;
int bestFit(int sizeInWords, void *list);
int worstFit(int sizeInWords, void *list);
class MemoryManager {
    public:
        class block {
                public:
                    int start;
                    void* ptr;
                    int size;
                    block(int start, int size,void* ptr){
                        this->start = start;
                        this->size = size;
                        this->ptr = ptr;
                    }
            };
    private:
        
        bool shutdownCalled = false;
        int bytesTotal=0;
        int wordSize=0;
        int sizeInWords=0;
        int wordsLeft=0;
        //void* memory;
        std::function<int(int, void *)> allocator;
        void* holeList=0;
        char* allocated=nullptr;
        map<int, block*> holes;
        map<int, block*> blocks;
    public:
        MemoryManager(unsigned wordSize, std::function<int(int, void *)> allocator) {
            this->wordSize = wordSize;
            this->allocator = (allocator);

        }
        ~MemoryManager(){
            if(shutdownCalled){
                return;
            }
            for(auto i: blocks){
                delete(i.second);
            }
            blocks.clear();
            for(auto i: holes){
                delete(i.second);
            }
            //cout<<"shutdow~n"<<endl;
            holes.clear();
            //::free(holeList);
            delete[] (allocated);
            allocated=nullptr;
        }
        void initialize(size_t sizeInWords);
        void shutdown();
        void *allocate(size_t sizeInBytes);
        void free(void *address);
        void setAllocator(std::function<int(int, void *)> allocator);
        int dumpMemoryMap(char *filename);
        void *getList();
        void *getBitmap();
        unsigned getWordSize();
        void *getMemoryStart();
        unsigned getMemoryLimit();
};
