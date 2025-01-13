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
static int getattr_callback(const char *path, struct stat *stbuf)
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
static int mknod_callback(const char *path, mode_t mode, dev_t dev)
{
    loadedWad->createFile(path);
    return 0;
}
static int mkdir_callback(const char *path, mode_t mode)
{
    loadedWad->createDirectory(path);
    return 0;
}
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
static int write_callback(const char *path, const char *buf, size_t size, off_t offset,
                          struct fuse_file_info *fi)
{
    //cout<<"\\n\n\n\n\nwrite callback\n\n\n\\n\n";
    int bytes = loadedWad->writeToFile(path, buf, size, offset);
   
    //cout<<"bytes: "<<bytes<<"    buffer"<<buf<<"\n";
    return bytes;
}


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
static int open_callback(const char *path, struct fuse_file_info *fi)
{
    if (!loadedWad->isContent(path))
    {
        return -ENOENT;
    }

    return 0;
}
static struct fuse_operations fuse_example_operations = {
    .getattr = getattr_callback,
    .mknod = mknod_callback,
    .mkdir = mkdir_callback,
    .open = open_callback,
    .read = read_callback,
    .write = write_callback,
    .readdir = readdir_callback,
};

int main(int argc, char *argv[])
{
    if(argc < 3){
        std::cout << "Not enough arguments.\n";
        exit(EXIT_SUCCESS);
    }

    std::string wadPath = argv[argc - 2];

    if(wadPath.at(0) != '/'){
        wadPath = std::string(get_current_dir_name()) + "/" + wadPath;
    }

    loadedWad = Wad::loadWad(wadPath);

    argv[argc - 2] = argv[argc - 1];
    argc--;

    return fuse_main(argc, argv, &fuse_example_operations, loadedWad);
}