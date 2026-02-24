#pragma once
#include "comHeader/array+arrayList.hpp"
#include "bTree.hpp"
#include "string.hpp"
namespace fileSystem{
    void init();
    void mkdir(const Core::String& rawPath);
    void mkfile(const Core::String& rawPath);
    void mkdir(const Core::String& path,uint32 owner);
    void mkfile(const Core::String& path,uint32 owner);
    Core::Array<Core::String> ls(const Core::String& rawPath);
    Core::Array<Core::String> find(const Core::String& pattern,const Core::String& startRawPath);
    void rename(const Core::String& oldRawPath,const Core::String& newRawPath);
    void move(const Core::String& sourceRawPath,const Core::String& destinationRawPath);
    void destroy(const Core::String& rawPath);
    bool exists(const Core::String& rawPath);
    bool isDir(const Core::String& rawPath);
    bool isFile(const Core::String& rawPath);


    void saveJournal();
    void makeSnapshot();
    void loadFromStorage();
}