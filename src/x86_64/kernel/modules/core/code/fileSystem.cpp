#include "../fileSystem.hpp"
#include <cstdlib>
#include "../display.hpp"

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
    [[maybe_unused]] Core::String getName(const Core::String& rawPath){
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

        bool isDirectory = false;

        uint64 size = 0;
        uint64 created = 0;
        uint64 modified = 0;

        uint32 ownerID = 0;
        uint32 group = 0;
        uint16 permissions = 0;
        uint32 flags = 0;

        uint64 blockStart = 0;
        volatile uint64 blockCount = 0;

        //SearchHelpers
        Core::String lastComponent;
        Core::String lowerLastComponent; 
        Core::Array<Core::String> components;
        Core::Array<Core::String> chainKeys;

        uint32 nameHash;
        uint32 checksum;
    };
    Core::Trees::bTree<Core::String, FileMeta> fileTree;
    Core::Hash::Map<Core::String, Core::Array<Core::String>> dirIndex;
}
namespace FileSystem{
    void addToIndex(const Core::String& path){
        Core::String parent = getParent(path);
        Core::Array<Core::String>* arr = dirIndex.get(parent);
        if(!arr){
            Core::Array<Core::String> newArr;
            newArr += path;
            dirIndex.insert(parent,newArr);
        }else
            *arr += path;
    }
    void init(){
        fileTree.init(64);
        dirIndex.init(128);
    }
    void mkdir(const Core::String& rawPath){
        LowLevelAccess::Hlt();
        Core::String path{normalize(rawPath)};
        FileMeta meta;
        meta.id = GetID();
        meta.isDirectory = true;
        meta.blockStart = GetDataBlock();
        //fileTree.add(path,meta);
        //addToIndex(path);
    }
    void mkfile(const Core::String& rawPath){
        Core::String path = normalize(rawPath);
        FileMeta meta;
        meta.id = GetID();
        meta.blockStart = GetDataBlock();
        fileTree.add(path,meta);
        addToIndex(path);
    }
    Core::Array<Core::String> ls(const Core::String& rawPath){
        Core::Array<Core::String>* arr = dirIndex.get(normalize(rawPath));
        if(!arr)
            return Core::Array<Core::String>(0);
        return Core::Array<Core::String>(*arr);
    }
    Core::Array<Core::String> find(const Core::String& pattern,const Core::String& startRawPath){
        Core::String pat = normalize(pattern);
        Core::Array<Core::String>* arr = dirIndex.get(normalize(startRawPath));
        if(!arr)
            return Core::Array<Core::String>(0);
        Core::ArrayList<Core::String> result;

        for(uint64 i=0;i< arr->length();++i){
            const Core::String& child = (*arr)[i];
            if(child.contains(pat))
                result += child;
        }
        return Core::Array<Core::String>(result);
    }
    void rename(const Core::String& oldRawPath,const Core::String& newRawPath){
        Core::String oldPath = normalize(oldRawPath);
        Core::String newPath = normalize(newRawPath);
        FileMeta* meta = fileTree.get(oldPath);
        if(!meta)
            return;
    }
    void move(const Core::String& sourceRawPath,const Core::String& destinationRawPath){
        (void)sourceRawPath;
        (void)destinationRawPath;
    }
    void destroy(const Core::String& rawPath){
        (void)rawPath;
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