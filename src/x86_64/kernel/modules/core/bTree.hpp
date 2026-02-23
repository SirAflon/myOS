#pragma once
#include "comHeader/array+arrayList.hpp"
namespace Core{
    template<typename keyType,typename mdataType>
    struct bTree{
        private:
            struct node{
                Core::ArrayList<mdataType> mdata;
                Core::ArrayList<keyType> keys;
                Core::ArrayList<uint32> childrenID;
                bool isLeaf = false;

                node() = default;
                void reserve(uint64 res){
                    mdata.reserve(res);
                    keys.reserve(res);
                    childrenID.reserve(res+1);
                }
            };
            Core::ArrayList<node> nodes;
            mdataType* searchRecursive(uint32 nodeID, const keyType& key) {
                node& n = nodes[nodeID];
                uint32 i = 0;
                while (i < n.keys.length() && key > n.keys[i])
                    i++;
                if (i < n.keys.length() && key == n.keys[i])
                    return &n.mdata[i];
                if (n.isLeaf) 
                    return nullptr;
                uint32 childID = n.childrenID[i];
                return searchRecursive(childID, key);
            }
            void insertIntoLeaf(uint32 nodeID,const keyType& key,const mdataType& meta){
                node& n = nodes[nodeID];
                uint32 i=0;
                while (i<n.keys.length()&&key> n.keys[i])
                    i++;
                n.keys.put(key, i);
                n.mdata.put(meta,i);
            }
            uint32 createNode(bool isLeaf){
                node n;
                n.isLeaf = isLeaf;
                n.keys.SetHardCapacity(maxKeys);
                n.mdata.SetHardCapacity(maxKeys);
                n.childrenID.SetHardCapacity(maxKeys+1);
                nodes += n;
                return nodes.length() -1;
            }
            void splitChild(uint32 parentID, uint32 childIndex) {
                node& parent = nodes[parentID];
                uint32 fullChildID = parent.childrenID[childIndex];
                node& fullChild = nodes[fullChildID];
                uint32 mid = maxKeys / 2;
                keyType midKey   = fullChild.keys[mid];
                mdataType midMeta = fullChild.mdata[mid];
                uint32 rightID = createNode(fullChild.isLeaf);
                node& right = nodes[rightID];
                for (uint32 i = mid + 1; i < fullChild.keys.length(); ++i) {
                    right.keys += fullChild.keys[i];
                    right.mdata += fullChild.mdata[i];
                }
                if (!fullChild.isLeaf)
                    for (uint32 i = mid + 1; i < fullChild.childrenID.length(); ++i) 
                        right.childrenID += fullChild.childrenID[i];
                fullChild.keys.shrinkTo(mid);
                fullChild.mdata.shrinkTo(mid);
                if (!fullChild.isLeaf)
                    fullChild.childrenID.shrinkTo(mid + 1);
                parent.keys.put(midKey, childIndex);
                parent.mdata.put(midMeta, childIndex);
                parent.childrenID.put(rightID, childIndex + 1);
            }

            void insertNonFull(uint32 nodeID,const keyType& key,const mdataType& meta){
                node& n = nodes[nodeID];
                if(n.isLeaf){
                    insertIntoLeaf(nodeID,key,meta);
                    return;
                }
                uint32 i=0;
                while (i<n.keys.length()&&key>n.keys[i])
                    i++;
                uint32 childID = n.childrenID[i];
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
            bTree(){
                nodes.init();
            };
            
            mdataType* searchFor(const keyType& key){
                if(nodes.length() == 0) return nullptr;
                return searchRecursive(0, key);
            }

            void add(const keyType& newKey,const mdataType& newMetaData){
                if(nodes.length() == 0){
                    uint32 rootID = createNode(true);
                    node& root = nodes[rootID];
                    root.keys += newKey;
                    root.mdata += newMetaData;
                    return;
                }
                if(nodes[0].keys.length() == maxKeys){
                    uint32 oldRootID = createNode(nodes[0].isLeaf);
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
    };
}
