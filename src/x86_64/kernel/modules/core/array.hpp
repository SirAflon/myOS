#pragma once
#include "comHeader/array+arrayList.hpp"
namespace Core{
    template<typename T>
    struct Array{
        friend struct ArrayList<T>;
        private:
            T* buffBuffer;
            uint64 buffLength;
            uint64 buffCapacity;
        public:
            //constructor
            Array()
            :buffBuffer(nullptr),
            buffLength(0),
            buffCapacity(0){}

            void init(uint64 cap){
                buffBuffer = (T*)Allocator::Buddy::alloc(sizeof(T)*cap);
                buffCapacity = cap;
                if(!Utilitys::Checks::isPrimary<T>()){
                    for(uint64 i=0;i<cap;i++)
                        buffBuffer[i].init();
                }
            }
            Array(uint64 cap)
            :buffBuffer((T*)Allocator::Buddy::alloc(sizeof(T)*cap)),
            buffLength(0),
            buffCapacity(cap){}

            explicit Array(const Core::ArrayList<T>& copy)
            :buffLength(copy.buffLength),
            buffCapacity(copy.buffCapacity),
            buffBuffer((T*)Allocator::Buddy::alloc(sizeof(T)*buffCapacity)){
                for(uint64 i=0;i<buffLength;++i)
                    new (&buffBuffer[i]) T(copy.buffBuffer[i]);
            }

            explicit Array(Core::ArrayList<T>&& move)
            :buffLength(move.buffLength),
            buffCapacity(move.buffLength),
            buffBuffer((T*)Allocator::Buddy::alloc(sizeof(T)*buffCapacity)){
                for(uint64 i=0;i<buffLength;++i)
                    new (&buffBuffer[i]) T(Utilitys::move(move.buffBuffer[i]));
                move.clear();
            }

            Array(const Array& copy)
            :buffLength(copy.buffLength),
            buffCapacity(copy.buffCapacity),
            buffBuffer((T*)Allocator::Buddy::alloc(sizeof(T)*buffCapacity)){
                for(uint64 i = 0; i < buffLength; i++)
                    new (&buffBuffer[i]) T(copy.buffBuffer[i]);
            }

            Array(Array&& move)
            :buffBuffer(move.buffBuffer),
            buffLength(move.buffLength),
            buffCapacity(move.buffCapacity){
                move.buffBuffer = nullptr;
                move.buffLength = 0;
                move.buffCapacity = 0;
            }
            //deconstructor
            ~Array(){
                if(buffBuffer){
                    for(uint64 i = 0; i < buffLength; i++)
                        buffBuffer[i].~T();
                    Allocator::Buddy::free(buffBuffer);
                }
            }
            //operations
            Array<T>& operator=(const Array& copy){
                if(this == &copy)
                    return *this;
                for(uint64 i = 0; i < buffLength; i++)
                    buffBuffer[i].~T();
                Allocator::Buddy::free(buffBuffer);

                buffLength = copy.buffLength;
                buffCapacity = copy.buffCapacity;
                buffBuffer = (T*)Allocator::Buddy::alloc(sizeof(T) * buffCapacity);
                for(uint64 i = 0; i < buffLength; i++)
                    new (&buffBuffer[i]) T(copy.buffBuffer[i]);
                return *this;
            }  
            Array<T>& operator=(Array<T>&& move){
                if(this == &move)
                    return *this;

                for(uint64 i = 0; i < buffLength; i++)
                    buffBuffer[i].~T();
                Allocator::Buddy::free(buffBuffer);

                buffBuffer = Utilitys::move(move.buffBuffer);
                buffLength = move.buffLength;
                buffCapacity = move.buffCapacity;

                move.buffBuffer = nullptr;
                move.buffLength = 0;
                move.buffCapacity = 0;
                return *this;
            }
            Array<T>& operator+=(const T& value){
                if(buffCapacity == buffLength)
                    return *this;
                new (&buffBuffer[buffLength]) T(value);
                buffLength++;
                return *this;
            }
            Array<T>& operator+=(T&& value) {
                if (buffCapacity == buffLength)
                    return *this;
                new (&buffBuffer[buffLength]) T(Utilitys::move(value));
                buffLength++;
                return *this;
            }

            Array<T>& operator+=(const Array<T>& value){
                uint64 needed = buffLength + value.buffLength;
                if(needed>buffCapacity)
                    needed=buffCapacity;
                uint64 toCopy = needed - buffLength;
                for(uint64 i = 0; i < toCopy; i++)
                    new (&buffBuffer[buffLength + i]) T(value.buffBuffer[i]);

                buffLength = needed;
                return *this;
            }
            Array<T>& operator+=(const Core::ArrayList<T>& value) {
                uint64 needed = buffLength + value.length();
                if (needed > buffCapacity)
                    needed = buffCapacity;
                uint64 toCopy = needed - buffLength;
                for (uint64 i = 0; i < toCopy; ++i)
                    new (&buffBuffer[buffLength + i]) T(value.buffer()[i]);
                buffLength = needed;
                return *this;
            }

            Array<T> operator+(const T& value){
                Array<T> tmp(*this);
                tmp += value;
                return tmp;
            }
            Array<T> operator+(const Array<T>& rhs){
                Array<T> tmp(*this);
                tmp += rhs;
                return tmp;
            }
            Array<T> operator+(const Core::ArrayList<T>& rhs){
                Array<T> tmp(*this);
                tmp += rhs;
                return tmp;
            }
            T& operator[](uint64 index){
                return buffBuffer[index];
            }
            const T& operator[](uint64 index) const {
                return buffBuffer[index];
            }
            bool operator==(const Array<T>& other) const {
                if(buffLength != other.buffLength)
                    return false;
                for(uint64 i = 0; i < buffLength; i++)
                    if(!(buffBuffer[i] == other.buffBuffer[i]))
                         return false;
                return true;
            }
            bool operator==(const Core::ArrayList<T>& other) const {
                if(buffLength != other.buffLength)
                    return false;
                for(uint64 i = 0; i < buffLength; i++)
                    if(!(buffBuffer[i] == other.buffBuffer[i]))
                         return false;
                return true;
            }
            bool operator!=(const Array<T>& other) const {
                return !(*this == other);
            }
            bool operator!=(const Core::ArrayList<T>& other) const {
                return !(*this == other);
            }
            operator bool()const{
                return !(buffLength==0);
            }
            uint64 length() const {
                return buffLength;
            }
            uint64 capacity() const {
                return buffCapacity;
            }
            T* buffer() {
                return buffBuffer;
            }
            Core::ArrayList<T> toArrayList(){
                return Core::ArrayList<T>(Utilitys::move(*this));
            }
            bool isEmpty()const{
                return (buffLength==0);
            }
            void clear(){
                for(uint64 i=0;i<buffLength;++i)
                    buffBuffer[i].~T();
                buffLength=0;
            }
            Optional<T> at(uint64 index) const {
                if (index >= buffLength)
                    return Optional<T>();
                return Optional<T>(buffBuffer[index]);
            }

            Optional<Array> at(uint64 index1,uint64 index2){
                if (index1 >= buffLength || index2 >= buffLength)
                    return Optional<Array>();
                uint64 start = (index1 < index2) ? index1 : index2;
                uint64 end = (index1 > index2) ? index1 : index2;
                Array tmp(end - start + 1);
                for (uint64 i = start; i <= end; ++i)
                    tmp += buffBuffer[i];
                return Optional<Array>(Utilitys::move(tmp));
            }
            Core::ArrayList<T> atArrayList(uint64 index1,uint64 index2){
                Optional<Array> opt = at(index1,index2);
                if(!opt)
                    return Core::ArrayList<T>(0);
                Array tmp = Utilitys::move(opt.value());
                return Core::ArrayList<T>(Utilitys::move(tmp));
            }
            void erase(uint64 index) {
                if (index >= buffLength)
                    return;
                buffBuffer[index].~T();
                for (uint64 i = index; i + 1 < buffLength; ++i) {
                    new (&buffBuffer[i]) T(Utilitys::move(buffBuffer[i + 1]));
                    buffBuffer[i + 1].~T();
                }
                --buffLength;
            }
            void erase(uint64 index1, uint64 index2) {
                if (index1 >= buffLength || index2 >= buffLength)
                    return;
                uint64 start = (index1 < index2) ? index1 : index2;
                uint64 end = (index1 > index2) ? index1 : index2;
                uint64 count = end - start + 1;
                for (uint64 i = start; i <= end; ++i)
                    buffBuffer[i].~T();
                for (uint64 i = start; i + count < buffLength; ++i) {
                    new (&buffBuffer[i]) T(Utilitys::move(buffBuffer[i + count]));
                    buffBuffer[i + count].~T();
                }
                buffLength -= count;
            }
            void deleteAt(uint64 index){
                if(index >= buffLength)
                    return;
                buffBuffer[index].~T();
            }
            void deleteAt(uint64 index1, uint64 index2){
                if (index1 >= buffLength || index2 >= buffLength)
                    return;
                uint64 start = (index1 < index2) ? index1 : index2;
                uint64 end = (index1 > index2) ? index1 : index2;
                for(uint64 i=start;i <= end;++i)
                    buffBuffer[i].~T();
            }
            Array cut(uint64 index1, uint64 index2) {
                Array tmp = at(index1, index2);
                erase(index1, index2);
                return tmp;
            }
            Core::ArrayList<T> cutArrayList(uint64 index1, uint64 index2) {
                Optional<Array> opt = at(index1, index2);
                if(!opt)
                    return ArrayList<T>(0);
                Array tmp = Utilitys::move(opt.value());
                erase(index1, index2);
                return Core::ArrayList<T>(Utilitys::move(tmp));
            }
            Array subArray(uint64 index, uint64 count) const {
                if (index >= buffLength)
                    return Array(0);
                if (index + count > buffLength)
                    count = buffLength - index;
                Array out(count);
                for (uint64 i = 0; i < count; ++i)
                    out += buffBuffer[index + i];
                return out;
            }
            Core::ArrayList<T> subArrayList(uint64 index, uint64 count) const {
                if (index >= buffLength)
                    return Core::ArrayList<T>(0);
                if (index + count > buffLength)
                    count = buffLength - index;
                Core::ArrayList<T> out(count);
                out.SetHardCapacity(false);
                for (uint64 i = 0; i < count; ++i)
                    out += buffBuffer[index + i];
                return out;
            }
            void put(const Array& arr, uint64 index) {
                uint64 len = arr.length();
                if (index > buffLength)
                    index = buffLength;

                uint64 maxInsert = buffCapacity - buffLength;
                if (len > maxInsert)
                    len = maxInsert;
                uint64 newLen = buffLength + len;
                for (uint64 i = buffLength; i > index; --i) {
                    new (&buffBuffer[i + len - 1]) T(Utilitys::move(buffBuffer[i - 1]));
                    buffBuffer[i - 1].~T();
                }
                for (uint64 i = 0; i < len; ++i)
                    new (&buffBuffer[index + i]) T(arr.buffBuffer[i]);
                buffLength = newLen;
            }
    };
}
