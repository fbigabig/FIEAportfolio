#include "Wad.h"

std::string Wad::getMagic()
{
    return magic;
}
bool Wad::isContent(const std::string &path)
{
    tree *result = startSearch(path); //gets the tree element matching the path given
    if (result == nullptr)
    {
        return false;
        cout << "Error: File does not exist" << endl;
    }
    return !result->isDir(); //if it exists, check if it is a dir and return the opposite of that
}
bool Wad::isDirectory(const std::string &path) //same as above function but returns the opposite value
{
    tree *result = startSearch(path);
    if (result == nullptr)
    {
        return false;
        cout << "Error: File does not exist" << endl;
    }
    return result->isDir();
}
int Wad::getSize(const std::string &path) //get the size of a file at path
{
    tree *result = startSearch(path);
    if (result == nullptr)
    {
        return -1;
    }
    if(result->isDir()){ //this function is for files only
        return -1;
    }
    int tmp = result->self.length;
    return (tmp >= 0 ? tmp : -1);
}
int Wad::getContents(const std::string &path, char *buffer, int length, int offset) //write the contents of a file to a buffer, return the length of the file
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
        if (offset >= result->self.length) { //the offset we would start at is outside the contents of the file, return 0 because nothing is written
            return 0;
        }
        fs.seekg(result->self.offset + offset, ios::beg);
        if (offset + length > result->self.length) //if we would try to read more than there is available
        {
            length = result->self.length - offset; //cap length at the length of the file minus the offset
        }
        fs.read(buffer, length); //read to the buffer
        return length;
    }
}
int Wad::getDirectory(const string &path, vector<string> *directoryVector) //fills a vector with the name of each file/dir in a dir, returns the number of elements in the dir
{
    tree *result = startSearch(path);
    if (result == nullptr || !result->isDir())
    {
        return -1;
    }
    directory *dir = (directory *)result; //cast result to the tree subclass dictionary
    for (auto e : dir->children)
    {
        directoryVector->push_back(e->self.name); //add the name of each child to the vector
    }
    return dir->children.size();
}

void Wad::createDirectory(const string &path)
{
    if (hasMap(path))
    {
        cout << "Error: no maps allowed" << endl; //this functionality was not required for the project so map creation is not implemented
        return;
    }

    string path2 = path;
    if (path.at(path.length() - 1) == '/')
    {
        path2 = path.substr(0, path.length() - 1); //eliminate extra / at the end
    }
    int idx = path2.find_last_of("/");
    string name = path2.substr(idx + 1); //name of the directory to be created
    string pathGo = path2.substr(0, idx); //path to follow before we create the directory

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
    tree *tmp;
    if (pathGo == "") //creating a directory in root
    {
        tmp = root;
    }
    else
    {
        tmp = startSearch(pathGo); //get the pointer for the directory we are making the new dir in
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

    directory *current = (directory *)tmp; //cast to dir

    int endLoc;
    if (current != root) //find the location in the list of descriptors that this directory's entry will be placed
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

    int bytes = 32; //16 * 2 bytes (descriptors for start + end)

    fs.flush();
    char *buf = makeBuf(bytes, endLoc);
    fs.seekg(endLoc, ios::beg);
    string name1 = name + "_START";
    string name2 = name + "_END";
    char *charName1 = new char[8];
    char *charName2 = new char[8];
    for (int i = 0; i < 8; i++) //convert strings to character arrays
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
    //write descriptors to the list in the Wad
    fs.write(zero, 4);
    fs.write(zero, 4);

    fs.write(charName1, 8);
    fs.write(zero, 4);
    fs.write(zero, 4);
    fs.write(charName2, 8);
    writeBuf(buf, bytes, endLoc);

    elementInserter(0, 0, name, endLoc, current, true); //add the dir to the tree
    updateDescNum(true);
}
void Wad::createFile(const string &path) //basically same as above but makes a file
{
    if (hasMap(path))
    {
        cout << "Error: no maps allowed" << endl;
        return;
    }

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

    tree *tmp;
    if (pathGo == "")
    {

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

    fs.flush();
    char *buf = makeBuf(16, endLoc); //one descriptor will be added
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

    fs.write(charName, 8);
    writeBuf(buf, 16, endLoc);

    elementInserter(0, 0, name, endLoc, current);
    updateDescNum(false);
}
int Wad::writeToFile(const string &path, const char *buffer, int length, int offset)
{

    string path2 = path;
    if (path.at(path.length() - 1) == '/')
    {
        path2 = path.substr(0, path.length() - 1);
    }
    int idx = path2.find_last_of("/");
    string pathGo = path2.substr(0, idx);

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
    int bytes = length + offset; //we need to write offset null bytes and length bytes so the total bytes is length + offset



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
    int descWhere = findDescriptorOffset(current->self.name, tmpVec); //find the descriptor for the file we are writing to
    if (descWhere == -1)
    {
        cout << "Error: File not found in descriptor list" << endl;
        return -1;
    }
    descWhere += descOffset;
    fs.flush();
    fs.seekg(descWhere, ios::beg); //go to the descriptor
    fs.write((char *)&descOffset, 4); //update the descriptor offset for this file to say that it's content is where the descriptors were previously, as we will be moving the list of descriptors to make room
    fs.write((char *)&bytes, 4);
    char *buf = makeBuf(bytes, descOffset); //store the list of descriptors in a buffer to write back later
    fs.seekg(descOffset, ios::beg); //go to where we want to write the file
    if (offset > 0)
    {

        char filler[offset];
        memset(filler, 0, offset);
        fs.write(filler, offset); //fill offset bytes with null data
    }

    fs.write(buffer, length); //write the content of the file to the wad
    current->self.offset = descOffset; //update its element in the tree
    current->self.length = bytes;

    writeBuf(buf, bytes, descOffset); //write the list of descriptors to the new descriptor offset location (descOffset+bytes)
    descOffset += bytes; //update descOffset
    fs.seekg(8, ios::beg);
    fs.write((char *)&descOffset, 4); //write the new descriptor offset to the start of the Wad
    fs.flush();
    return length;
}
