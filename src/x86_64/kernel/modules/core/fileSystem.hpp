#pragma once
#include "comHeader/array+arrayList.hpp"
#include "bTree.hpp"
#include "string.hpp"
namespace fileSystem{
    void init();
    void mkdir(const Core::String& path);
    void mkfile(const Core::String& path);
    Core::Array<Core::String> ls(const Core::String& path);
    Core::Array<Core::String> find(const Core::String& pattern,const Core::String& startPath);
    void rename(const Core::String& oldPath,const Core::String& newPath);
    void move(const Core::String& sourcePath,const Core::String& destinationPath);
    void destroy(const Core::String& path);
    bool exists(const Core::String& path);
    bool isDir(const Core::String& path);
    bool isFile(const Core::String& path);


    void saveJournal();
    void makeSnapshot();
    void loadFromStorage();
}