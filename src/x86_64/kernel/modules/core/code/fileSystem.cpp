#include "../fileSystem.hpp"


namespace{
    uint64 ids =0;
    uint64 GetID(){
        ids++;
        return ids-1;
    }
    uint64 GetDataBlock(){
    }
    struct FileMeta {
        uint64 id;
        uint64 parentID;

        bool isDirectory;

        uint64 size;
        uint64 created;
        uint64 modified;

        uint32 ownerID;
        uint32 group;
        uint16 permissions;
        uint32 flags;

        uint64 dataBlockStart;
        uint64 dataBlockCount;

        //SearchHelpers
        Core::String lastComponent;
        Core::String lowerLastComponent; 
        Core::Array<Core::String> components;
        Core::Array<Core::String> chainKeys;

        uint32 nameHash;
        uint32 checksum;
    };
    Core::Trees::bTree<Core::String, FileMeta> fileTree{};
}
namespace fileSystem{
    void init(){

    }
    void mkdir(const Core::String& path){
        FileMeta meta;
        meta.id = GetID();
        meta.isDirectory = true;
        meta.size = 0;
        meta.dataBlockStart = GetDataBlock();
        meta.dataBlockCount = 1;
        meta.ownerID = 0;
        fileTree.add(path,meta);
    }
    void mkFile(const Core::String& path){
        FileMeta meta;
        meta.id = GetID();
        meta.isDirectory = false;
        meta.size = 0;
        meta.dataBlockStart = GetDataBlock();
        meta.dataBlockCount = 1;
        meta.ownerID = 0;
        fileTree.add(path,meta);
    }
    Core::Array<Core::String> ls(const Core::String& path){

    }
    Core::Array<Core::String> find(const Core::String& pattern,const Core::String& startPath){

    }
    void rename(const Core::String& oldPath,const Core::String& newPath){

    }
    void move(const Core::String& sourcePath,const Core::String& destinationPath){

    }
    void destroy(const Core::String& path){

    }
    bool exists(const Core::String& path){

    }
    bool isDir(const Core::String& path){

    }
    bool isFile(const Core::String& path){

    }
    void saveJournal(){

    }
    void makeSnapshot(){

    }
    void loadFromStorage(){

    }
}