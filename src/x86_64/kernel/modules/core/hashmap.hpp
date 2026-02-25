#pragma once
#include "array.hpp"
#include "hashTypes.hpp"
#include "comHeader/array+arrayList.hpp"
#include "math.hpp"
#include "string.hpp"
#include "utilitys.hpp"
namespace Core{
    namespace Hash{
        template<typename Key, typename Value>
        struct Map{
            private:
                struct Entry{
                    Key key;
                    Value value;
                    bool used=false;
                    bool tombstone=false;
                };
                void rehash(){
                    uint64 newCap= Core::Math::nextPow2(table.length()*2);
                    Core::Array<Entry> old{Utilitys::move(table)};
                    table.reserve(newCap);
                    for(uint64 i=0;i<newCap;i++)
                        table += Entry{};
                    count =0;
                    for(uint64 i=0;i<old.length();i++)
                        if(old[i].used && !old[i].tombstone)
                            insertNoRehash(old[i].key,old[i].value);
                }
                void insertNoRehash(const Key& key, const Value& value){
                    uint64 mask = table.length() - 1;
                    uint64 index = Core::Hash::hash(hashType, &key, 0) & mask;

                    while(true){
                        Entry& ent = table[index];
                        if(!ent.used || ent.tombstone){
                            ent.key = key;
                            ent.value = value;
                            ent.used = true;
                            ent.tombstone = false;
                            count++;
                            return;
                        }
                        index = (index + 1) & mask;
                    }
                }

                Core::ArrayList<Entry> table{};
                uint64 count=0;
                float loadFactor = 0.7f;
                hashTypes hashType = hashTypes::xxHash;
                template<typename T>
                uint64 hashKey(const T& key,hashTypes type){
                    return Core::Hash::hash(type,key,sizeof(Key));
                }
                uint64 hashKey(const String* key,hashTypes type){
                    return Core::Hash::hash(type,key->buffer(),key->length());
                }
            public:
                Map()=default;
                Map(uint64 initialCapacity){
                    init(initialCapacity);
                }
                void init(uint64 initialCapacity){
                    count=0;
                    uint64 cap = Core::Math::nextPow2(initialCapacity);
                    table.reserve(cap);
                    for(uint64 i=0;i<cap;++i)
                        table += Entry();
                }

                void insert(const Key& key,const Value& value){
                    if(static_cast<double>(count) / static_cast<double>(table.length())>loadFactor)
                        rehash();
                    uint64 mask = table.length() -1;
                    uint64 index = hashKey(key) & mask;

                    while(true){
                        Entry& ent = table[index];
                        if(!ent.used||ent.tombstone){
                            ent.key = key;
                            ent.value = value;
                            ent.used = true;
                            ent.tombstone = false;
                            count++;
                            return;
                        }
                        if(ent.key == key){
                            ent.value = value;
                            return;
                        }
                        index = (index+1)&mask;
                    }
                }
                Value* get(const Key& key){
                    uint64 mask = table.length()-1;
                    uint64 index = hashKey(key) & mask;
                    while(true){
                        Entry& ent = table[index];
                        if(!ent.used && !ent.tombstone)
                            return nullptr;
                        if(ent.used && ent.key == key)
                            return &ent.value;
                        index = (index+1) & mask;
                    }
                }
                bool contains(const Key& key){
                    return get(key);
                }
                void remove(const Key& key){
                    uint64 mask = table.length()-1;
                    uint64 index = hashKey(key) & mask;
                    while(true){
                        Entry& ent = table[index];
                        if(!ent.used && !ent.tombstone)
                            return;
                        if(ent.used && ent.key == key){
                            ent.used=false;
                            ent.tombstone=true;
                            count--;
                            return;
                        }
                        index = (index+1) & mask;
                    }
                }
                void SetUsedHashType(hashTypes newHashType){
                    switch(newHashType){
                        case hashTypes::DJB2:
                            return;
                        case hashTypes::DJB2a:
                            return;
                        case hashTypes::Adler32:
                            return;
                        default:
                            hashType = newHashType;
                            rehash();
                    }
                }
                const hashTypes& GetUsedHashType(){
                    return hashType;
                }
        };
    }
}