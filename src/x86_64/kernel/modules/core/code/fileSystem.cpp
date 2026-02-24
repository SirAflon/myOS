#include "../fileSystem.hpp"


namespace{
    uint64 ids =0;
    uint64 GetID(){
        ids++;
        return ids-1;
    }
    uint64 GetDataBlock(){
        return 0;
    }
    Core::String normalize(const Core::String& path){
        if(path.length() == 0)
            return Core::String("/");
        Core::String out;
        if(path[0] != '/')
            out +="/";
        bool lastWasSlash = false;
        for(char c : path){
            if(c=='/'){
                if(!lastWasSlash)
                    out+='/';
                lastWasSlash=true;
            }else{
                out += c;
                lastWasSlash=false;
            }
        }
        if(out.length() > 1 && out[out.length() -1] == '/')
            out = out.subString(0, out.length() -1);
        return out;
    }
    Core::String getParent(const Core::String& rawPath){
        Core::String path = normalize(rawPath);
        if(path == "/" || path.length() ==0)
            return Core::String("/");
        uint64 lastSlash = path.lastIndexOf('/');
        if(lastSlash == 0)
            return Core::String("/");
        return path.subString(0, lastSlash);
    }
    Core::String getName(const Core::String& rawPath){
        Core::String path = normalize(rawPath);
        if(path =="/")
            return Core::String("/");
        uint64 lastSlash = path.lastIndexOf('/');
        if(lastSlash == 0)
            return path;
        return path.subString(lastSlash, path.length()-1);
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
    void mkdir(const Core::String& rawPath){
        Core::String path = normalize(rawPath);
        FileMeta meta;
        meta.id = GetID();
        meta.isDirectory = true;
        meta.size = 0;
        meta.dataBlockStart = GetDataBlock();
        meta.dataBlockCount = 1;
        meta.ownerID = 0;
        fileTree.add(path,meta);
    }
    void mkFile(const Core::String& rawPath){
        Core::String path = normalize(rawPath);
        FileMeta meta;
        meta.id = GetID();
        meta.isDirectory = false;
        meta.size = 0;
        meta.dataBlockStart = GetDataBlock();
        meta.dataBlockCount = 1;
        meta.ownerID = 0;
        fileTree.add(path,meta);
    }
    Core::Array<Core::String> ls(const Core::String& rawPath){
        return Core::Array<Core::String>(0);
    }
    Core::Array<Core::String> find(const Core::String& pattern,const Core::String& startRawPath){
        return Core::Array<Core::String>(0);
    }
    void rename(const Core::String& oldRawPath,const Core::String& newRawPath){
        Core::String oldPath = normalize(oldRawPath);
        Core::String newPath = normalize(newRawPath);
        FileMeta* meta = fileTree.get(oldPath);
        if(!meta)
            return;
    }
    void move(const Core::String& sourceRawPath,const Core::String& destinationRawPath){

    }
    void destroy(const Core::String& rawPath){
    }
    bool exists(const Core::String& rawPath){
        return fileTree.get(normalize(rawPath));
    }
    bool isDir(const Core::String& rawPath){
        FileMeta* meta = fileTree.get(normalize(rawPath));
        return (meta)? meta->isDirectory:false;
    }
    bool isFile(const Core::String& rawPath){
        FileMeta* meta = fileTree.get(normalize(rawPath));
        return (meta)? !meta->isDirectory:false;
    }
    void saveJournal(){

    }
    void makeSnapshot(){

    }
    void loadFromStorage(){

    }
}