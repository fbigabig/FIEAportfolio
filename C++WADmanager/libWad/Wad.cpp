#include "Wad.h"

std::string Wad::getMagic()
{
    return magic;
}
bool Wad::isContent(const std::string &path)
{
    tree *result = startSearch(path);
    if (result == nullptr)
    {
        return false;
        cout << "Error: File does not exist" << endl;
    }
    return !result->isDir();
}
bool Wad::isDirectory(const std::string &path)
{
    tree *result = startSearch(path);
    if (result == nullptr)
    {
        return false;
        cout << "Error: File does not exist" << endl;
    }
    return result->isDir();
}
int Wad::getSize(const std::string &path)
{
    tree *result = startSearch(path);
    if (result == nullptr)
    {
        return -1;
    }
    if(result->isDir()){
        return -1;
    }
    int tmp = result->self.length;
    return (tmp >= 0 ? tmp : -1);
}
int Wad::getContents(const std::string &path, char *buffer, int length, int offset)
{
    tree *result = startSearch(path);
    if (result == nullptr)
    {
        cout << "Error: File does not exist" << endl;
        return -1;
    }
    else if (result->isDir())
    {
        cout << "Error: Cannot read from directory" << endl;
        return -1;
    }
    else
    {
        if (offset >= result->self.length)
            return 0;
        fs.seekg(result->self.offset + offset, ios::beg);
        if (offset + length > result->self.length)
        {
            length = result->self.length - offset;
        }
        fs.read(buffer, length);
        return length;
    }
}
int Wad::getDirectory(const string &path, vector<string> *directoryVector)
{
    tree *result = startSearch(path);
    if (result == nullptr || !result->isDir())
    {
        return -1;
    }
    directory *dir = (directory *)result;
    for (auto e : dir->children)
    {
        directoryVector->push_back(e->self.name);
    }
    return dir->children.size();
}

void Wad::createDirectory(const string &path) // need to make stuff at end of dir not start
{
    if (hasMap(path))
    {
        cout << "Error: no maps allowed" << endl;
        return;
    }
    //cout << "Creating directory at " << path << endl;
    string path2 = path;
    if (path.at(path.length() - 1) == '/')
    {
        path2 = path.substr(0, path.length() - 1);
    }
    int idx = path2.find_last_of("/");
    string name = path2.substr(idx + 1);
    string pathGo = path2.substr(0, idx);

    if (name.length() > 2)
    {
        cout << "Error: Directory name is too long" << endl;
        return;
    }
    if (startSearch(path) != nullptr)
    {
        cout << "Error: Dir already exists" << endl;
        return;
    }
    //cout << path << " " << path2 << " " << name << " " << pathGo << endl;
    tree *tmp;
    if (pathGo == "")
    {
        /*
        cout<<"Error: Cannot create in root directory"<<endl;
        return;
        */
        tmp = root;
    }
    else
    {
        tmp = startSearch(pathGo);
    }

    if (tmp == nullptr)
    {
        cout << "Error: Directory does not exist" << endl;
        return;
    }

    if (!tmp->isDir())
    {
        cout << "Error: Path is not a directory" << endl;
        return;
    }

    directory *current = (directory *)tmp;
    //cout << "current: " << current->self.name << endl;
    int endLoc;
    if (current != root)
    {
        vector<string> tmpVec = {};

        string tmp = "";
        for (int l = 1; l < pathGo.length(); l++)
        {
            if (pathGo.at(l) == '/')
            {
                tmpVec.push_back(tmp);
                tmp = "";
            }
            else
            {
                tmp += pathGo.at(l);
            }
        }
        if (tmp != "")
            tmpVec.push_back(tmp);
        endLoc = descOffset + findDescriptorOffset(current->self.name + "_END", tmpVec);
    }
    else
    {
        endLoc = getEnd();
    }
    //cout << "endLoc: " << endLoc << "end: " << end << endl;
    int bytes = 32;
    // addbytes(bytes, current);

    // fix here:
    fs.flush();
    char *buf = makeBuf(bytes, endLoc);
    fs.seekg(endLoc, ios::beg);
    string name1 = name + "_START";
    string name2 = name + "_END";
    char *charName1 = new char[8];
    char *charName2 = new char[8];
    for (int i = 0; i < 8; i++)
    {
        if (i < name1.length())
        {
            charName1[i] = name1.at(i);
        }
        else
        {
            break;
        }
        if (i < name2.length())
        {
            charName2[i] = name2.at(i);
        }
    }
    //cout << "Writing: " << name1 << " " << name2 << " at " << endLoc << " " << fs.tellg() << endl;
    fs.write(zero, 4);
    fs.write(zero, 4);

    fs.write(charName1, 8);
    fs.write(zero, 4);
    fs.write(zero, 4);
    fs.write(charName2, 8);
    writeBuf(buf, bytes, endLoc);
    //cout << "added" << endl;
    elementInserter(0, 0, name, endLoc, current, true);
    updateDescNum(true);
}
void Wad::createFile(const string &path) // update these to use new directory stuff
{
    if (hasMap(path))
    {
        cout << "Error: no maps allowed" << endl;
        return;
    }
    //cout << "Creating file at " << path << endl;
    string path2 = path;
    if (path.at(path.length() - 1) == '/')
    {
        path2 = path.substr(0, path.length() - 1);
    }
    int idx = path2.find_last_of("/");
    string name = path2.substr(idx + 1);
    if (name.length() > 8)
    {
        cout << "Error: File name is too long" << endl;
        return;
    }
    if (startSearch(path) != nullptr)
    {
        cout << "Error: File already exists" << endl;
        return;
    }
    string pathGo = path2.substr(0, idx);
    //cout << path << " " << path2 << " " << name << " " << pathGo << endl;

    tree *tmp;
    if (pathGo == "")
    {
        /*
        cout<<"Error: Cannot create in root directory"<<endl;
        return;
        */
        tmp = root;
    }
    else
    {
        tmp = startSearch(pathGo);
    }
    if (tmp == nullptr)
    {
        cout << "Error: Directory does not exist" << endl;
        return;
    }

    if (!tmp->isDir())
    {
        cout << "Error: Path is not a directory" << endl;
        return;
    }

    directory *current = (directory *)tmp;
    //cout << "current: " << current->self.name << endl;
    int endLoc;
    if (current != root)
    {
        vector<string> tmpVec = {};
        string tmp = "";
        for (int l = 1; l < pathGo.length(); l++)
        {
            if (pathGo.at(l) == '/')
            {
                tmpVec.push_back(tmp);
                tmp = "";
            }
            else
            {
                tmp += pathGo.at(l);
            }
        }
        if (tmp != "")
            tmpVec.push_back(tmp);
        endLoc = descOffset + findDescriptorOffset(current->self.name + "_END", tmpVec);
    }
    else
    {
        endLoc = getEnd();
    }
    //cout << "endLoc: " << endLoc << "   end:   " << end << endl;
    fs.flush();
    char *buf = makeBuf(16, endLoc);
    fs.seekg(endLoc, ios::beg);
    fs.write(zero, 4);
    fs.write(zero, 4);
    char charName[8];
    memset(charName, 0, 8);
    for (int i = 0; i < 8; i++)
    {
        if (i < name.length())
        {
            charName[i] = name.at(i);
        }
        else
        {
            break;
        }
    }
    //cout << "Writing: " << name << " char name  " << charName << " at " << endLoc << " " << fs.tellg() << endl;
    fs.write(charName, 8);
    writeBuf(buf, 16, endLoc);
    //cout << "\n\n\nddedFILE MADE at  "<<path<<" \n\n" << endl;
    elementInserter(0, 0, name, endLoc, current);
    updateDescNum(false);
}
int Wad::writeToFile(const string &path, const char *buffer, int length, int offset)
{
    //cout<<"\n\n\nwrite to file\n\n";
    //cout << path << endl;
    string path2 = path;
    if (path.at(path.length() - 1) == '/')
    {
        path2 = path.substr(0, path.length() - 1);
    }
    int idx = path2.find_last_of("/");
    string pathGo = path2.substr(0, idx);
    //cout << pathGo << endl;
    tree *current = startSearch(path2);
    if (current == nullptr)
    {
        cout << "Error: File does not exist" << endl;
        return -1;
    }
    if (current->isDir())
    {
        cout << "Error: Cannot write to directory" << endl;
        return -1;
    }

    if (current->self.length > 0)
    {
        cout << "Error: File is not empty" << endl;
        return 0;
    }
    int bytes = length + offset;

    // addbytes(bytes, current);

    vector<string> tmpVec = {};
    string tmp = "";
    for (int l = 1; l < pathGo.length(); l++)
    {
        if (pathGo.at(l) == '/')
        {
            tmpVec.push_back(tmp);
            tmp = "";
        }
        else
        {
            tmp += pathGo.at(l);
        }
    }
    if (tmp != "")
        tmpVec.push_back(tmp);
    int descWhere = findDescriptorOffset(current->self.name, tmpVec);
    if (descWhere == -1)
    {
        cout << "Error: File not found in descriptor list" << endl;
        return -1;
    }
    descWhere += descOffset;
    fs.flush();
    fs.seekg(descWhere, ios::beg);
    fs.write((char *)&descOffset, 4);
    fs.write((char *)&bytes, 4);
    char *buf = makeBuf(bytes, descOffset);
    fs.seekg(descOffset, ios::beg);
    if (offset > 0)
    {

        char filler[offset];
        memset(filler, 0, offset);
        fs.write(filler, offset);
    }
    //cout << "descOffset: " << descOffset << "end:" << end << " bytes: " << bytes << " buffer: " << buffer << " legtnh + offset" << length << " " << offset << " where" << descWhere << " num " << descNum << endl;
    fs.write(buffer, length);
    current->self.offset = descOffset;

    current->self.length = bytes;
    writeBuf(buf, bytes, descOffset);
    descOffset += bytes;
    fs.seekg(8, ios::beg);
    fs.write((char *)&descOffset, 4);
    fs.flush();
    return length;
}