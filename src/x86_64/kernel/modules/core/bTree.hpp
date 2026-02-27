#pragma once
#include "comHeader/array+arrayList.hpp"
namespace Core{
    namespace Trees{
        template<typename keyType,typename mdataType>
        struct bTree{
            private:
                struct node{
                    Core::ArrayList<mdataType> mdata;
                    Core::ArrayList<keyType> keys;
                    Core::ArrayList<uint64> childrenID;
                    bool isLeaf = false;

                    node() = default;
                    void reserve(uint64 res){
                        mdata.reserve(res);
                        keys.reserve(res);
                        childrenID.reserve(res+1);
                    }
                    void init(){
                        mdata.init();
                        keys.init();
                        childrenID.init();
                    }
                };
                Core::ArrayList<node> nodes;
                mdataType* searchRecursive(uint64 nodeID, const keyType& key) {
                    node& n = nodes[nodeID];
                    uint64 i = 0;
                    while (i < n.keys.length() && key > n.keys[i])
                        i++;
                    if (i < n.keys.length() && key == n.keys[i])
                        return &n.mdata[i];
                    if (n.isLeaf) 
                        return nullptr;
                    uint64 childID = n.childrenID[i];
                    return searchRecursive(childID, key);
                }
                template<typename Function>
                void transverseRecursive(uint64 nodeID,Function func){
                    node& n=nodes[nodeID];
                    if(n.isLeaf){
                        for(uint64 i=0;i<n.keys.length();++i)
                            func(n.keys[i],n.mdata[i]);
                    }else{
                        for(uint64 i=0;i<n.keys.length();++i){
                            transverseRecursive( n.childrenID[i],func);
                            func(n.keys[i],n.mdata[i]);
                        }
                        transverseRecursive(n.childrenID[n.keys.length()],func);
                    }
                }
                void insertIntoLeaf(uint64 nodeID,const keyType& key,const mdataType& meta){
                    node& n = nodes[nodeID];
                    uint64 i=0;
                    while (i<n.keys.length()&&key> n.keys[i])
                        i++;
                    n.keys.put(key, i);
                    n.mdata.put(meta,i);
                }
                uint64 createNode(bool isLeaf){
                    node n;
                    n.isLeaf = isLeaf;
                    n.keys.SetHardCapacity(maxKeys);
                    n.mdata.SetHardCapacity(maxKeys);
                    n.childrenID.SetHardCapacity(maxKeys+1);
                    nodes += n;
                    return nodes.length() -1;
                }
                void splitChild(uint64 parentID, uint64 childIndex) {
                    node& parent = nodes[parentID];
                    uint64 fullChildID = parent.childrenID[childIndex];
                    node& fullChild = nodes[fullChildID];
                    uint64 mid = maxKeys / 2;
                    keyType midKey   = fullChild.keys[mid];
                    mdataType midMeta = fullChild.mdata[mid];
                    uint64 rightID = createNode(fullChild.isLeaf);
                    node& right = nodes[rightID];
                    for (uint64 i = mid + 1; i < fullChild.keys.length(); ++i) {
                        right.keys += fullChild.keys[i];
                        right.mdata += fullChild.mdata[i];
                    }
                    if (!fullChild.isLeaf)
                        for (uint64 i = mid + 1; i < fullChild.childrenID.length(); ++i) 
                            right.childrenID += fullChild.childrenID[i];
                    fullChild.keys.shrinkTo(mid);
                    fullChild.mdata.shrinkTo(mid);
                    if (!fullChild.isLeaf)
                        fullChild.childrenID.shrinkTo(mid + 1);
                    parent.keys.put(midKey, childIndex);
                    parent.mdata.put(midMeta, childIndex);
                    parent.childrenID.put(rightID, childIndex + 1);
                }

                void insertNonFull(uint64 nodeID,const keyType& key,const mdataType& meta){
                    node& n = nodes[nodeID];
                    if(n.isLeaf){
                        insertIntoLeaf(nodeID,key,meta);
                        return;
                    }
                    uint64 i=0;
                    while (i<n.keys.length()&&key>n.keys[i])
                        i++;
                    uint64 childID = n.childrenID[i];
                    if(nodes[childID].keys.length() == maxKeys){
                        splitChild(nodeID, i);
                        if(key>n.keys[i])
                            childID = n.childrenID[i+1];
                        else
                            childID = n.childrenID[i];
                    }
                    insertNonFull(childID, key, meta);
                }
                uint64 maxKeys=3;

            public:
                bTree() = default;
            
                mdataType* get(const keyType& key){
                    if(nodes.length() == 0) 
                        return nullptr;
                    return searchRecursive(0, key);
                }

                void init(uint64 startCap){
                    nodes.init(startCap);
                }

                void add(const keyType& newKey,const mdataType& newMetaData){
                    if(nodes.length() == 0){
                        uint64 rootID = createNode(true);
                        node& root = nodes[rootID];
                        root.keys += newKey;
                        root.mdata += newMetaData;
                        return;
                    }
                    if(nodes[0].keys.length() == maxKeys){
                        uint64 oldRootID = createNode(nodes[0].isLeaf);
                        nodes[oldRootID] = nodes[0];
                        node& newRoot = nodes[0];
                        newRoot.isLeaf=false;
                        newRoot.keys.clear();
                        newRoot.mdata.clear();
                        newRoot.childrenID.clear();
                        newRoot.childrenID += oldRootID;
                        splitChild(0, 0);
                        insertNonFull(0, newKey, newMetaData);
                    } else{
                        insertNonFull(0, newKey, newMetaData);
                    }
                }
                template<typename Function>
                void forEach(Function func){
                    if(nodes.length() == 0)
                        return;
                    transverseRecursive<Function>(0,func);
                }
        };
    }
}