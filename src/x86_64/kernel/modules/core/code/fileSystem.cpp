#include "../fileSystem.hpp"
namespace fileSystem{
    void init(){

    }
    void mkdir(const Core::String& path){

    }
    void mkFile(const Core::String& path){

    }
}
namespace{
    struct FileMeta {
        uint64 id;
        uint64 parentID;

        bool isDirectory;

        uint64 size;
        uint64 created;
        uint64 modified;

        uint32 owner;
        uint32 group;
        uint16 permissions;
        uint32 flags;

        uint64 dataBlockStart;
        uint64 dataBlockCount;

        //SearchHelpers
        Core::String lastComponent;
        Core::String lowerLastComponent; 
        Core::ArrayList<Core::String> components;
        Core::ArrayList<Core::String> chainKeys;

        uint32 nameHash;
        uint32 checksum;
    };
    Core::bTree<Core::String, FileMeta> fileTree{};
}