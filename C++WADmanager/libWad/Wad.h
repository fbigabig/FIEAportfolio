//libWad header file, implements function prototypes, definies variables, and implements some of the functionality of the library (mainly helper functions, while the cpp file contains the main functions).
//Also includes tree and directory subclasses (tree is the base class and used for individual files, directory inherits from tree and adds onto it)
#include <string>
#include <fstream>
#include <iostream>
#include <vector>
#include <map>
#include <stdio.h>
#include <cstring>

using namespace std;
class Wad
{
private:
    string magic;
    int endsWith(string const &str) //check if a string is the start of end of a dir
    {
        int idx = str.find_last_of("_");
        if (idx == string::npos)
        {
            return -1;
        }

        if (str.substr(idx + 1).compare("END") == 0)
        {
            return 2;
        }

        if (str.substr(idx + 1).compare("START") == 0)
        {
            return 1;
        }

        return -1;
    }

public:

    int descNum; //number of descriptors
    int descOffset; //offset of the list of descriptors
    fstream fs;
    string path;
    char *zero = new char[4]{0, 0, 0, 0}; //used for writing null data

    int getEnd(){ //find how long the file opened by fstream fs is
        int g = fs.tellg();
        fs.seekg(0, ios::end);
        int end = fs.tellg();
        fs.seekg(g, ios::beg);
        return end;
    }


    struct element //contains basic information about a file
    {
        int offset;
        int length;
        string name;
        bool isMap = false;
    };



    struct tree;
    struct directory;
    struct tree //contains information about a file, as well as the directory it is stored in
    {
        directory *parent;

        element self;
        tree(element e, directory *up)
        {
            self = e;
            parent = up;
        }
        tree()
        {
            self = element();
            self.name = "rt";
            self.length = 0;
            self.offset = 0;
            parent = nullptr;
        }
        virtual bool isDir() //this class will always return false when this function is called, its used to distinguish between it and its child class
        {
            return false;
        }
    };
    struct directory : tree //extends tree class functionality to contain information about a directory and its children
    {
        vector<tree *> children;
        directory(element e, directory *up) : tree(e, up) //calls the tree(e, up) constructor
        {
        }
        directory() //default constructor
        {
        }
        tree *addElement(element e) //create a tree instance to represent a file in this directory and add it to the list of elements in this directory
        {
            tree *tmp = new tree(e, this);
            children.push_back(tmp);
            return tmp;
        }
        directory *addDirectory(element e) //create a directory instance to represent another directory inside this one and add it to the list of elements in this directory
        {
            directory *tmp = new directory(e, this);
            children.push_back(tmp);
            return tmp;
        }
        bool isDir() //distinguishes this class from the tree class
        {
            return true;
        }
    };



private:
    int findDescriptorOffset(string name, vector<string> &folders) //find the location of the description inside the WAD TOC
    {
        fs.flush();
        bool found = false;
        bool rootFolder = false;
        if (folders.size() == 0) //folders vectors lists the folders we have to go into to find out file, so if its empty we're looking in the root folder

        {
            found = true;
            rootFolder = true;
        }

        fs.seekg(descOffset, ios::beg);
        char *buffer = new char[8];

        for (int i = 0; i < descNum; i++) //go thru every descriptor in the list of descriptors
        {
            fs.read(buffer, 8);
            fs.read(buffer, 8);

            string tmp = string(buffer, 8);

            for (int j = 0; j < 8; j++)
            {
                if (tmp.at(j) == 0)
                {
                    tmp = tmp.substr(0, j);

                    break;
                }
            }

            if (!found)
            {
                if (tmp.compare(folders[0] + "_START")==0)
                {
                    if (folders.size() > 1)
                    {
                        folders.erase(folders.begin()); //we moved into a folder we wanted, move on to next folder
                    }
                    else
                    {
                        found = true; //we found the folder we wanted
                    }
                }
            }
            else //we're in the right folder, look for the file
            {
                if (tmp.compare(name) == 0) //found it
                {
                    return (i) * 16; //16B/descriptor* i descriptors
                }
                else if (!rootFolder)
                {
                    if (tmp.compare(folders[0] + "_END") == 0) //couldn't find it
                    {
                        return -1;
                    }
                }
            }
        }
        //cout << "Not found" << endl;
        return -1;
    }
    tree *search(tree *t, string path) //recursively look through tree nodes for a file specified by path
    {
        if (path == "")
        {
            return nullptr;
        }
        int find1 = path.find("/");
        string current;
        bool lff = false;
        if (find != string::npos)
        {
            int find2 = path.find1("/", find1 + 1);
            if (find2 != string::npos) //there are multiple / in the path, meaning that we have to look for directories then a file
            {
                current = path.substr(find1 + 1, find2 - find1 - 1);
                if (find2 + 1 == path.length())
                {
                    lff = true; //we are looking for a file
                }
                else
                {
                    lff = false; //we have to find a directory before we can look for a file
                }
            }
            else //no need to find dirs, just look for the file
            {
                current = path.substr(find1 + 1); //get the name we are currently looking for
                lff = true;
            }
        }
        else //no / so we're just looking for a file
        {
            current = path;
            lff = true;
        }

        if (t->isDir()) //if the current tree node we're looking at is a directory
        {
            for (auto i : ((directory *)t)->children) //look thru the files/dirs in the dir
            {

                int result = i->self.name.compare(current);
                if (result == 0) //if the file/dir has the name we want
                {
                    if (lff)
                    {
                        return i; //if we were looking for a file, return a pointer to that file
                    }
                    else
                    {
                        tree *result = search(i, path.substr(find + 1)); //we found a directory we wanted, now search it for the next thing we want
                        if (result != nullptr) //we found the file, recurse back up with the result
                        {
                            return result;
                        }
                    }
                }
            }
        }
        return nullptr; //failed to find it, return
    }

    bool hasMap(string s) //check if a string is a map-type file which contains the next 10 files (ex E1M1)
    {
        for (int i = 0; i + 3 < s.length(); i++)
        {
            if (s.at(i) == 'E' && s.at(i + 2) == 'M')
            {
                return true;
            }
        }
        return false;
    }

    char *makeBuf(int bytes, int where) //make a buffer in the data to create room for stuff to be written
    {
        int end = getEnd();
        if (where == end)
        {
            //cout << "Where is end" << endl;
            return nullptr;
        }
        char *buffer = new char[end - where];
        fs.flush();
        fs.seekg(where, ios::beg);
        fs.read(buffer, end - where);


        return buffer; //return a pointer to where the data was written
    }
    void writeBuf(char *buffer, int bytes, int where) //write x bytes from the buffer at where+bytes bytes in the wad
    {
        int endv = getEnd();
        if (buffer == nullptr)
        {
            //buffer is null
            return;
        }


        fs.flush();
        fs.seekg(where + bytes, ios::beg);
        fs.write(buffer, endv - where);
        delete[] buffer;
    }

    void updateDescNum(bool dir) //update the number of descriptors
    {

        descNum += (dir ? 2 : 1);
        fs.seekg(4, ios::beg);
        fs.write((char *)&descNum, 4);
        fs.seekg(4, ios::beg);
        char fgj[4];
        fs.read(fgj, 4);
    }

    void elementInserter(int length, int offset, string name, int descWhere, directory *current, bool dir = false) //take in information, make an element from it, and then add it to the directory passed in
    {
        element newElement;
        newElement.length = length;
        newElement.offset = offset;
        newElement.name = name;
        // newElement.descWhere = descWhere;
        if (dir)
        {
            current->addDirectory(newElement);
        }
        else
        {
            current->addElement(newElement);
        }
    }

public:
    directory *root;

    tree *startSearch(string path) //start a search if what is being searched for is not root, otherwise, return root
    {
        if (path == "/")
        {
            return root;
        }
        return search(root, path);
    }

    Wad(const string &path) //construct a Wad instance from a file
    {
        vector<element> elements;
        descNum = 0;
        descOffset = 0;
        magic = "";
        elements = vector<element>();
        this->path = path;
        fs.open(path, ios::in | ios::out | ios::binary);
        if (!fs.is_open())
        {
            cout << "Error opening file" << endl;
        }
        char *buffer = new char[4];
        fs.read(buffer, 4);
        magic = string(buffer, 4);
        fs.read(buffer, 4);
        descNum = *(int *)buffer;
        fs.read(buffer, 4);
        descOffset = *(int *)buffer;
        fs.seekg(descOffset, ios::beg); //go to the list of descriptors
        for (int i = 0; i < descNum; i++) //read each descriptor
        {
            fs.read(buffer, 4);
            int offset = *(int *)buffer;
            fs.read(buffer, 4);
            int length = *(int *)buffer;
            char *name = new char[8];
            fs.read(name, 8);
            element e;
            e.offset = offset;
            e.length = length;
            e.name = string(name, 8); //turn a descriptor into an element
            for (int i = 0; i < 8; i++)
            {
                if (e.name.at(i) == 0) //cut out extra zeros
                {
                    e.name = e.name.substr(0, i);
                    break;
                }
            }
            elements.push_back(e);
        }

        root = new directory(); //set up our tree
        directory *current = root;
        int counter = 0;
        for (auto e : elements) //add each element to the tree
        {

            if (counter > 0) //counter tracks if we're in a map right now
            {
                (current)->addElement(e); //add the current element to the map directory
                counter--;
                if (counter == 0)
                {
                    current = current->parent; //exit the map directory
                }
                continue;
            }

            int result = endsWith(e.name); //check if the current element is the start or end of a dir
            if (result == 2) //e is the end of the current dir, go up one level
            {

                current = current->parent;
            }
            else if (result == 1) //e is the start of a dir
            {
                e.name = e.name.substr(0, e.name.find("_START")); //cut out the start text from the name of the dir
                directory *tmp = (current)->addDirectory(e); //create the dir element

                current = tmp;
            }
            else if (e.name.at(0) == 'E' && e.name.at(2) == 'M') //e is a map, read in the next 10 elements and add them to the map's directory
            {
                counter = 10;
                e.isMap = true;
                directory *tmp = (current)->addDirectory(e);

                current = tmp;
            }
            else
            {

                (current)->addElement(e); //e is a file, add it to the current directory
            }

        }

    }
    static Wad *loadWad(const std::string &path) //load a new Wad at the path specified
    {
        Wad *myWad = new Wad(path);
        return myWad;
    }
    static void printTree(tree *t, int depth = 0) //print the entire tree, used for debugging
    {
        for (int i = 0; i < depth; i++)
        {
            cout << "  ";
        }
        cout << t->self.name << endl;
        if (t->isDir())
        {

            for (auto e : ((directory *)t)->children)
            {
                printTree(e, depth + 1);
            }
        }
    }
    //function prototypes
    std::string getMagic();
    bool isContent(const std::string &path);
    bool isDirectory(const std::string &path);
    int getSize(const std::string &path);
    int getContents(const std::string &path, char *buffer, int length, int offset = 0);
    int getDirectory(const string &path, vector<string> *directory);
    void createDirectory(const std::string &path);
    void createFile(const string &path);
    int writeToFile(const string &path, const char *buffer, int length, int offset = 0);

    ~Wad() //destructor
    {
        delTree(root);
        fs.close();
    }
    void delTree(tree *t) //recursively delete every element in the tree.
    {

        if (t->isDir())
        {
            for (auto e : ((directory *)t)->children)
            {
                delTree(e);
            }
        }
        delete t;
    }
};
