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
    int endsWith(string const &str)
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
    int descNum;
    int descOffset;
    fstream fs;
    string path;
    char *zero = new char[4]{0, 0, 0, 0};
    struct element
    {
        int offset;
        // int descWhere;
        int length;
        string name;
        bool isMap = false;
    };
    struct tree;
    struct directory;
    struct tree
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
        virtual bool isDir()
        {
            return false;
        }
    };
    int getEnd(){
        int g = fs.tellg();
        fs.seekg(0, ios::end);
        int end = fs.tellg();
        fs.seekg(g, ios::beg);
        return end;
    }
    struct directory : tree
    {
        vector<tree *> children;
        directory(element e, directory *up) : tree(e, up)
        {
        }
        directory()
        {
        }
        tree *addElement(element e)
        {
            // cout << e.name << " " << e.length << " " << e.offset << " added by   "<< self.name<<endl;
            // children[e.name];
            // cout<<"a"<<endl;
            tree *tmp = new tree(e, this);
            children.push_back(tmp);
            return tmp;
            // cout<<"Added: "<<e.name<<" "<<e.length<<" "<<e.offset<<endl;
        }
        directory *addDirectory(element e)
        {
            directory *tmp = new directory(e, this);
            children.push_back(tmp);
            return tmp;
        }
        bool isDir()
        {
            return true;
        }
    };

private:
    int findDescriptorOffset(string name, vector<string> &folders)
    {
        fs.flush();
        bool found = false;
        bool rootFolder = false;
        if (folders.size() == 0)
        {
            found = true;
            rootFolder = true;
        }
        //cout << "Searching for: " << name << endl;
        fs.seekg(descOffset, ios::beg);
        //cout << "Desc offset: " << descOffset << " " << fs.tellg() << "descNUM" << descNum << " math" << descNum * 16 << " end" << end << " math2 " << descOffset + descNum * 16 << endl;
        char *buffer = new char[8];

        for (int i = 0; i < descNum; i++)
        {
            fs.read(buffer, 8);
            fs.read(buffer, 8);
            // cout<<"Checking: "<<string(buffer, 8)<<endl;
            string tmp = string(buffer, 8);
            //cout << "Checking: " << tmp << endl;
            for (int j = 0; j < 8; j++)
            {
                if (tmp.at(j) == 0)
                {
                    tmp = tmp.substr(0, j);

                    break;
                }
            }
            //cout << "Checking: " << tmp << "   compare  "<<tmp.compare(name)<<endl;
           // for(auto c:tmp) {
           //     cout<<c<<" ";
           // }
            /*cout<<endl;
            for(auto c:name) {
                cout<<c<<" ";
            }
            cout<<endl;
            cout << folders.size() << endl;
            cout<<found<<" "<<rootFolder<<endl;
            if(!rootFolder){
                for(auto f: folders){
                    cout<<f<<endl;
                }
            }*/
            if (!found)
            {
                if (tmp.compare(folders[0] + "_START")==0)
                {
                    if (folders.size() > 1)
                    {
                        folders.erase(folders.begin());
                    }
                    else
                    {
                        //cout<<"Found"<<endl;
                        //for(auto f: folders){
                        //    cout<<f<<endl;
                        //}
                        found = true;
                    }
                }
            }
            else
            {
                if (tmp.compare(name) == 0)
                {
                    return (i) * 16;
                }
                else if (!rootFolder)
                {
                    if (tmp.compare(folders[0] + "_END") == 0)
                    {
                        //cout << "left folder" << endl;
                        return -1;
                    }
                }
            }
        }
        //cout << "Not found" << endl;
        return -1;
    }
    tree *search(tree *t, string path)
    {
        if (path == "")
        {
            return nullptr;
        }
        int find = path.find("/");
        string current;
        bool lff = false;
        if (find != string::npos)
        {
            int find2 = path.find("/", find + 1);
            if (find2 != string::npos)
            {
                current = path.substr(find + 1, find2 - find - 1);
                if (find2 + 1 == path.length())
                {
                    lff = true;
                }
                else
                {
                    lff = false;
                }
            }
            else
            {
                current = path.substr(find + 1);
                lff = true;
            }
        }
        else
        {
            current = path;
            lff = true;
        }
        // cout << "Current: " << current << " lff: "<<lff<<endl;

        if (t->isDir())
        {
            for (auto i : ((directory *)t)->children)
            {

                int result = i->self.name.compare(current);
                // cout<<"Checking: "<<i.first<<" "<<current<<" "<<result<<endl;
                if (result == 0)
                {
                    // cout<<"Found: "<<i.first<<" "<<current<<endl;
                    if (lff)
                    {
                        return i;
                    }
                    else
                    {
                        tree *result = search(i, path.substr(find + 1));
                        if (result != nullptr)
                        {
                            return result;
                        }
                    }
                }
            }
        }
        return nullptr;
    } /*
     void addbytes(int bytes, tree *current)
     {
         // fix whoopsie
         bool found = false;
         for (auto t : nodesInOrder)
         {
             if (found)
             {
                 t->self.offset += bytes;
                 t->self.descWhere += bytes;
             }
             if (t == current)
             {
                 found = true;
             }
         }
     }
     */
    bool hasMap(string s)
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

    char *makeBuf(int bytes, int where)
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

        //cout << "Buffer: " << buffer << " " << end << " " << where << "tell  " << fs.tellg() << endl;
        return buffer;
    }
    void writeBuf(char *buffer, int bytes, int where)
    {
        int end = getEnd();
        if (buffer == nullptr)
        {
            //cout << "Buffer is null" << endl;
            return;
        }

        //cout << "Buffer: " << buffer << " " << end << " " << where << endl;
        fs.flush();
        fs.seekg(where + bytes, ios::beg);
        fs.write(buffer, end - where);
        delete[] buffer;
    }

    void updateDescNum(bool dir)
    {
        
        //cout << "\n\nUpdating desc num  " << descNum<<endl;
        descNum += (dir ? 2 : 1);
        //cout<<"New desc num: "<<descNum<<endl;
        fs.seekg(4, ios::beg);
        fs.write((char *)&descNum, 4);
        fs.seekg(4, ios::beg);
        char fgj[4];
        fs.read(fgj, 4);
        //cout<<"Read: "<<*(int*)fgj<<endl<<endl;
    } /*
     void dumpAll() {
         fs.seekg(0, ios::beg);
         //header
         fs.write(magic.c_str(), 4);
         fs.write((char *)&descNum, 4);
         fs.write((char *)&descOffset, 4);
     }*/

    void elementInserter(int length, int offset, string name, int descWhere, directory *current, bool dir = false)
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

    tree *startSearch(string path)
    {
        if (path == "/")
        {
            return root;
        }
        return search(root, path);
    }

    Wad(const string &path)
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
        fs.seekg(descOffset, ios::beg);
        for (int i = 0; i < descNum; i++)
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
            e.name = string(name, 8);
            //cout << e.name << endl;
            for (int i = 0; i < 8; i++)
            {
                if (e.name.at(i) == 0)
                {
                    e.name = e.name.substr(0, i);
                    break;
                }
            }
            elements.push_back(e);
        }
        //end = descOffset + descNum * 16;
        //cout << "End: " << end << " " << fs.tellg() << "    " << descOffset << "    " << descNum * 16 << endl;
        root = new directory();
        directory *current = root;
        int counter = 0;
        for (auto e : elements)
        {
            //cout << "Adding: " << e.name << "   current   " << current->self.name << endl;

            if (counter > 0)
            {
                (current)->addElement(e);
                counter--;
                if (counter == 0)
                {
                    current = current->parent;
                }
                continue;
            }

            int result = endsWith(e.name);
            if (result == 2)
            {
                // cout << "end" << endl;

                // current->end = last;

                current = current->parent;
            }
            else if (result == 1)
            {
                e.name = e.name.substr(0, e.name.find("_START"));
                directory *tmp = (current)->addDirectory(e);

                current = tmp;
            }
            else if (e.name.at(0) == 'E' && e.name.at(2) == 'M')
            {
                counter = 10;
                e.isMap = true;
                directory *tmp = (current)->addDirectory(e);

                current = tmp;
            }
            else
            {

                (current)->addElement(e);
            }
            // cout << "Added element: " << e.name << "  counter: "<<counter<<" result: "<<result<<endl;
        }

        //printTree(root);
    }
    static Wad *loadWad(const std::string &path)
    {
        Wad *myWad = new Wad(path);
        return myWad;
    }
    static void printTree(tree *t, int depth = 0)
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
    std::string getMagic();
    bool isContent(const std::string &path);
    bool isDirectory(const std::string &path);
    int getSize(const std::string &path);
    int getContents(const std::string &path, char *buffer, int length, int offset = 0);
    int getDirectory(const string &path, vector<string> *directory);
    void createDirectory(const std::string &path);
    void createFile(const string &path);
    int writeToFile(const string &path, const char *buffer, int length, int offset = 0);
    /*
    void printAll()
    {
        cout << "Magic: " << magic << endl;
        cout << "Number of elements: " << descNum << endl;
        cout << "Offset of elements: " << descOffset << endl;
        for (int i = 0; i < nodesInOrder.size(); i++)
        {
            cout << nodesInOrder[i]->self.name << endl;
        }
    }
    */
    ~Wad()
    {
        //printTree(root);
        delTree(root);
        fs.close();
    }
    void delTree(tree *t)
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