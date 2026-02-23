#pragma once
#include "dataTypes.hpp"
#include "hashTypes.hpp"
#include "lowLevelAccess.hpp"
#include "allocator.hpp"
namespace Core {
    struct String{
        private:
            char* strBuffer;
            uint64 strLength;
            uint64 strCapacity;
            bool hardCap = false;
            void pinit(const char*,uint64,uint64);
        public:
            String();
            explicit String(uint64);
            explicit String(const char*);
            explicit String(const char*,uint64);
            void init();
            void init(const char*,uint64);
            void init(const char*);
            void init(uint64);

            String(const String&);
            String(String&&);
            String& operator=(const String&);
            String& operator=(String&&);
            String& operator=(char);
            String& operator=(const char*);
            String& operator+=(char);
            String& operator+=(const char*);
            String& operator+=(const String&);
            String operator+(char)const;
            String operator+(const char*)const;
            String operator+(const String&)const;
            char& operator[](uint64);
            const char& operator[](uint64) const;
            bool operator==(const String&)const;
            bool operator!=(const String&)const;
            bool operator<(const String&)const;
            bool operator>(const String&)const;
            operator bool()const;

            ~String();

            uint64 length() const;
            uint64 capacity() const;
            void SetCapacity(uint64);
            bool hardCapacity() const;
            void SetHardCapacity(bool);
            void SetHardCapacity(uint64);
            char* buffer();
            const char* buffer() const;
            void clear();
            void shrinkToFit();
            char getAt(uint64)const;
            String getAt(uint64,uint64)const;
            bool isEmpty()const;
            bool startsWith(const char*)const;
            bool endsWith(const char*)const;
            int compare(const String&)const;
            void erase(uint64);
            void erase(uint64,uint64);
            String cut(uint64,uint64);
            int64 find(char c)const;
            String subString(uint64,uint64)const;
            void put(String,uint64);
            void resize(uint64,char);
            void reserve(uint64);
            void trim();
            void trimRight();
            void trimLeft();
            void replace(char,char);
            void toUpper();
            void toLower();
            bool equalsIgnoreCase(const String&)const;
            uint64 hash() const;
            uint64 hash(hashTypes)const;
            bool contains(char c)const;
            void popBack();
            void popBack(uint64);

            static uint64 CalcLength(const char*);
    };
}