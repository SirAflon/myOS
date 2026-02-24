#pragma once
#include "comHeader/array+arrayList.hpp"
namespace Core{
    namespace Trees{
        template<typename keyType,typename metaData>
        struct binaryTree{
            private:
                struct node{
                    keyType key;
                    metaData data;
                    uint64 leftID;
                    uint64 rightID;
                    node()=default;
                };
                metaData* recusivSearch(keyType key,uint64 startIndex){
                    node* ptr = nodes[startIndex];
                    if(ptr.leftID==0&&ptr.rightID==0)
                        return nullptr;
                    if(key == ptr.key)
                        return ptr.data;
                    if(key<nodes[ptr.rightID]){
                        return recusivSearch(key,ptr.leftID);
                    }else{
                        return recusivSearch(key,ptr.leftID);
                    }
                }
                Core::ArrayList<node> nodes;
                Core::ArrayList<metaData> metaDatas;
            public:
                binaryTree()=default;

                void add(keyType key, metaData meta){

                }
                metaData* getData(keyType key){

                }
        };
    }
}