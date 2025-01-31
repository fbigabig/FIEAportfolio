//Implements FUSE api to mount WAD files to the linux filesystem using the libWAD library to handle the WAD files.
#include <cstring>
#include <iostream>
#include <sys/stat.h>
#define FUSE_USE_VERSION 26
#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "../libWad/Wad.h"
Wad *loadedWad;
static int getattr_callback(const char *path, struct stat *stbuf) //get the attributes of a file
{
    memset(stbuf, 0, sizeof(struct stat));

    if (loadedWad->isDirectory(path))
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
        return 0;
    }

    if (loadedWad->isContent(path))
    {
        //cout<<"/n/nhere/n/n";
        stbuf->st_mode = S_IFREG | 0777;
        stbuf->st_nlink = 1;
        stbuf->st_size = loadedWad->getSize(path);
        //cout<<"size: "<<stbuf->st_size<<"\n\n";
        return 0;
    }

    return -ENOENT;
}
//Create a file inside the WAD
static int mknod_callback(const char *path, mode_t mode, dev_t dev)
{
    loadedWad->createFile(path);
    return 0;
}
//Create a directory inside the WAD
static int mkdir_callback(const char *path, mode_t mode)
{
    loadedWad->createDirectory(path);
    return 0;
}
//Attempt to read a file in the WAD
static int read_callback(const char *path, char *buf, size_t size, off_t offset,
                         struct fuse_file_info *fi)
{
    int bytes = loadedWad->getContents(path, buf, size, offset);
    if (bytes > 0)
    {

        return bytes;
    }

    return -ENOENT;
}
//Attempt to write to a file in the WAD
static int write_callback(const char *path, const char *buf, size_t size, off_t offset,
                          struct fuse_file_info *fi)
{
    int bytes = loadedWad->writeToFile(path, buf, size, offset);
    return bytes;
}

//Attempt to get the contents of a directory in the WAD
static int readdir_callback(const char *path, void *buf, fuse_fill_dir_t filler,
                            off_t offset, struct fuse_file_info *fi)
{
    (void)offset;
    (void)fi;

    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    vector<string> contents = vector<string>();
    int getDirRet = loadedWad->getDirectory(path, &contents);
    for (auto e : contents)
    {
        filler(buf, e.c_str(), NULL, 0);
    }

    return 0;
}
//Open a directory in the WAD. Fails if the path given leads to a file instead of a dir
static int open_callback(const char *path, struct fuse_file_info *fi)
{
    if (!loadedWad->isContent(path))
    {
        return -ENOENT;
    }

    return 0;
}
//Sets up the fuse operations implemented by this Daemon
static struct fuse_operations fuse_example_operations = {
    .getattr = getattr_callback,
    .mknod = mknod_callback,
    .mkdir = mkdir_callback,
    .open = open_callback,
    .read = read_callback,
    .write = write_callback,
    .readdir = readdir_callback,
};
//Handles running this daemon from the command line
int main(int argc, char *argv[])
{
    if(argc < 3){ //this daemon takes in 3 command line arguments: the path to the WAD, the -s flag to run single-threaded, and the directory to mount the WAD
        std::cout << "Not enough arguments.\n";
        exit(EXIT_SUCCESS);
    }

    std::string wadPath = argv[argc - 2]; //get the path to the wad

    if(wadPath.at(0) != '/'){
        wadPath = std::string(get_current_dir_name()) + "/" + wadPath; //Add a / if needed
    }

    loadedWad = Wad::loadWad(wadPath); //Open the WAD with libWad

    argv[argc - 2] = argv[argc - 1]; //Set up the args for the fuse_main function to handle (only 2 args need to be passed in)
    argc--;

    return fuse_main(argc, argv, &fuse_example_operations, loadedWad); //call the FUSE library's main function and pass in the relevant arguments
}
