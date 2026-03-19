#pragma once
#include "comHeader/array+arrayList.hpp"
namespace Core{
    template<typename T>
    struct ArrayList{
        friend struct Array<T>;
        private:
            T* buffBuffer;
            uint64 buffLength =0;
            uint64 buffCapacity =0;
            bool hardCap = false;
        public:
            ArrayList()
            :buffBuffer(nullptr),
            buffLength(0),
            buffCapacity(0),
            hardCap(false){}

            void init(){
                buffBuffer = (T*)Allocator::Buddy::alloc(sizeof(T)*4);
                buffCapacity = 4;
            }
            void init(uint64 cap){
                buffBuffer = (T*)Allocator::Buddy::alloc(sizeof(T)*cap);
                buffCapacity = cap;
                hardCap = true;
            }
            void init(const Core::Array<T>& copy){
                buffLength = copy.length;
                buffCapacity = copy.buffLength;
                buffBuffer =(T*)Allocator::Buddy::alloc(sizeof(T)*buffCapacity);
                for(uint64 i=0;i<buffLength;++i)
                    new (&buffBuffer[i]) T(copy.buffBuffer[i]);
            }
            void init(Core::Array<T>&& move){
                buffLength = move.length;
                buffCapacity = move.buffLength;
                buffBuffer=(T*)Allocator::Buddy::alloc(sizeof(T)*buffCapacity);
                for(uint64 i=0;i<buffLength;++i)
                    new (&buffBuffer[i]) T(Utilitys::move(move.buffBuffer[i]));
                move.clear();
            }
            explicit ArrayList(const Core::Array<T>& copy)
            :buffBuffer(nullptr),
            buffLength(copy.buffLength),
            buffCapacity(copy.buffLength),
            hardCap(false){
                if(buffCapacity > 0){
                    buffBuffer = (T*)Allocator::Buddy::alloc(sizeof(T)*buffCapacity);
                    for(uint64 i=0;i<buffLength;++i)
                        new (&buffBuffer[i]) T(copy.buffBuffer[i]);
                }
            }
            explicit ArrayList(Core::Array<T>&& move)
            :buffBuffer(nullptr),
            buffLength(move.buffLength),
            buffCapacity(move.buffLength),
            hardCap(false){
                if(buffCapacity==0)
                    return;
                buffBuffer = (T*)Allocator::Buddy::alloc(sizeof(T)*buffCapacity);
                for(uint64 i=0;i<buffLength;++i)
                    new (&buffBuffer[i]) T(Utilitys::move(move.buffBuffer[i]));
                move.clear();
            }
            ArrayList(uint64 cap)
            :buffBuffer((T*)Allocator::Buddy::alloc(sizeof(T)*cap)),
            buffLength(0),
            buffCapacity(cap),
            hardCap(true){}

            ArrayList(const ArrayList& copy)
            :buffBuffer(nullptr),
            buffLength(copy.buffLength),
            buffCapacity(copy.buffCapacity),
            hardCap(copy.hardCap){
                buffBuffer = (T*)Allocator::Buddy::alloc(sizeof(T)*buffCapacity);
                for(uint64 i = 0; i < buffLength; i++)
                    new (&buffBuffer[i]) T(copy.buffBuffer[i]);
            }

            ArrayList(ArrayList&& move)
            :buffBuffer(move.buffBuffer),
            buffLength(move.buffLength),
            buffCapacity(move.buffCapacity),
            hardCap(move.hardCap){
                move.buffBuffer = nullptr;
                move.buffLength = 0;
                move.buffCapacity = 0;
                move.hardCap = false;
            }
            
            ~ArrayList(){
                if(buffBuffer){
                    for(uint64 i = 0; i < buffLength; i++)
                        buffBuffer[i].~T();
                    Allocator::Buddy::free(buffBuffer);
                }
            }
            ArrayList<T>& operator=(const ArrayList& copy){
                if(this == &copy)
                    return *this;
                for(uint64 i = 0; i < buffLength; i++)
                    buffBuffer[i].~T();
                Allocator::Buddy::free(buffBuffer);

                buffLength = copy.buffLength;
                buffCapacity = copy.buffCapacity;
                hardCap = copy.hardCap;

                buffBuffer = (T*)Allocator::Buddy::alloc(sizeof(T) * buffCapacity);
                for(uint64 i = 0; i < buffLength; i++)
                    new (&buffBuffer[i]) T(copy.buffBuffer[i]);
                return *this;
            }
            ArrayList<T>& operator=(ArrayList<T>&& move){
                if(this == &move)
                    return *this;

                for(uint64 i = 0; i < buffLength; i++)
                    buffBuffer[i].~T();
                Allocator::Buddy::free(buffBuffer);

                buffBuffer = Utilitys::move(move.buffBuffer);
                buffLength = move.buffLength;
                buffCapacity = move.buffCapacity;
                hardCap = move.hardCap;

                move.buffBuffer = nullptr;
                move.buffLength = 0;
                move.buffCapacity = 0;
                move.hardCap = false;
                return *this;
            }
            ArrayList<T>& operator+=(const T& value){
                if(hardCap && buffLength == buffCapacity)
                    return *this;

                if(buffLength == buffCapacity){
                    uint64 newCap = (buffCapacity == 0) ? 4 : buffCapacity * 2;
                    T* newBuff = (T*)Allocator::Buddy::alloc(sizeof(T) * newCap);

                    if constexpr (Utilitys::Checks::isPrimary<T>()){
                        for(uint64 i = 0; i < buffLength; i++)
                            newBuff[i] = buffBuffer[i];
                    } else {
                        for(uint64 i=0;i<buffLength;i++){
                            new (&newBuff[i]) T(Utilitys::move(buffBuffer[i]));
                            buffBuffer[i].~T();
                        }
                    }
                    Allocator::Buddy::free(buffBuffer);
                    buffBuffer = newBuff;
                    buffCapacity = newCap;
                }

                if constexpr (Utilitys::Checks::isPrimary<T>())
                    buffBuffer[buffLength] = value;
                else 
                    new (&buffBuffer[buffLength]) T(value);
                buffLength++;
                return *this;
            }
            ArrayList<T>& operator+=(const ArrayList<T>& value){
                if(this == &value){
                    ArrayList<T> copy(value);
                    return *this += copy;
                }
                uint64 needed = buffLength + value.buffLength;

                if(hardCap && needed > buffCapacity)
                    needed = buffCapacity;

                if(!hardCap && needed > buffCapacity){
                    uint64 newCap = needed * 2;
                    T* newBuff = (T*)Allocator::Buddy::alloc(sizeof(T) * newCap);

                    for(uint64 i = 0; i < buffLength; i++){
                        new (&newBuff[i]) T(Utilitys::move(buffBuffer[i]));
                        buffBuffer[i].~T();
                    }

                    Allocator::Buddy::free(buffBuffer);
                    buffBuffer = newBuff;
                    buffCapacity = newCap;
                }

                uint64 toCopy = needed - buffLength;
                for(uint64 i = 0; i < toCopy; i++)
                    new (&buffBuffer[buffLength + i]) T(value.buffBuffer[i]);

                buffLength = needed;
                return *this;
            }
            ArrayList<T>& operator+=(ArrayList<T>&& move){
                if(this == &move){
                    ArrayList<T> copy(move);
                    return *this += copy;
                }
                uint64 needed = buffLength + move.buffLength;

                if(hardCap && needed > buffCapacity)
                    needed = buffCapacity;

                if(!hardCap && needed > buffCapacity){
                    uint64 newCap = needed * 2;
                    T* newBuff = (T*)Allocator::Buddy::alloc(sizeof(T) * newCap);

                    for(uint64 i = 0; i < buffLength; i++){
                        new (&newBuff[i]) T(Utilitys::move(buffBuffer[i]));
                        buffBuffer[i].~T();
                    }

                    Allocator::Buddy::free(buffBuffer);
                    buffBuffer = newBuff;
                    buffCapacity = newCap;
                }

                uint64 toCopy = needed - buffLength;
                for(uint64 i = 0; i < toCopy; i++)
                    new (&buffBuffer[buffLength + i]) T(Utilitys::move(move.buffBuffer[i]));

                buffLength = needed;
                return *this;
            }
            ArrayList<T> operator+(const Core::Array<T>& rhs) const {
                ArrayList<T> temp(*this);
                temp.reserve(buffLength+rhs.buffLength);
                for(uint64 i = 0; i < rhs.buffLength; i++)
                    temp += rhs.buffBuffer[i];
                return temp;
            }
            ArrayList<T> operator+(const ArrayList<T>& rhs) const {
                ArrayList<T> temp(*this);
                temp.reserve(buffLength+rhs.buffLength);
                for(uint64 i = 0; i < rhs.buffLength; i++)
                    temp += rhs.buffBuffer[i];
                return temp;
            }
            ArrayList<T> operator+(const T& rhs) const {
                ArrayList<T> temp(*this);
                temp += rhs;
                return temp;
            }
            T& operator[](uint64 index){
                return buffBuffer[index];
            }
            const T& operator[](uint64 index) const {
                return buffBuffer[index];
            }
            bool operator==(const ArrayList<T>& other) const {
                if(buffLength != other.buffLength)
                    return false;
                for(uint64 i = 0; i < buffLength; i++)
                    if(!(buffBuffer[i] == other.buffBuffer[i]))
                         return false;
                return true;
            }
            bool operator==(const Core::Array<T>& other) const {
                if(buffLength != other.buffLength)
                    return false;
                for(uint64 i = 0; i < buffLength; i++)
                    if(!(buffBuffer[i] == other.buffBuffer[i]))
                         return false;
                return true;
            }
            bool operator!=(const ArrayList<T>& other) const {
                return !(*this == other);
            }
            bool operator!=(const Array<T>& other) const {
                return !(*this == other);
            }
            operator bool()const{
                return !(buffLength==0);
            }
            //functions
            uint64 length() const {
                return buffLength;
            }
            uint64 capacity() const {
                return buffCapacity;
            }
            T* buffer() {
                return buffBuffer;
            }
            const T* buffer() const {
                return buffBuffer;
            }
            void SetCapacity(uint64 newCap) {
                if (hardCap)
                    return;
                T* newBuff = (T*)Allocator::Buddy::alloc(sizeof(T) * newCap);
                uint64 toCopy = (buffLength < newCap) ? buffLength : newCap;
                for (uint64 i = 0; i < toCopy; ++i) {
                    new (&newBuff[i]) T(Utilitys::move(buffBuffer[i]));
                    buffBuffer[i].~T();
                }
                for (uint64 i = toCopy; i < buffLength; ++i)
                    buffBuffer[i].~T();
                Allocator::Buddy::free(buffBuffer);
                buffBuffer = newBuff;
                buffCapacity = newCap;
                buffLength = toCopy;
            }
            bool hardCapacity() const {
                return hardCap;
            }
            void SetHardCapacity(bool set) {
                hardCap = set;
            }
            void SetHardCapacity(uint64 newCap) {
                if(newCap == buffCapacity){
                    hardCap=true;
                    return;
                }
                hardCap = false;
                SetCapacity(newCap);
                hardCap = true;
            }
            void reserve(uint64 res) {
                if (res <= buffCapacity)
                    return;
                SetCapacity(res);
            }
            void clear() {
                for (uint64 i = 0; i < buffLength; ++i)
                    buffBuffer[i].~T();
                buffLength = 0;
            }
            void shrinkToFit() {
                if (buffLength == buffCapacity)
                    return;
                SetCapacity(buffLength);
            }
            Optional<T> at(uint64 index) const {
                if(index >= buffLength)
                    return Optional<T>();
                return Optional<T>(buffBuffer[index]);
            }

            Optional<ArrayList<T>> at(uint64 index1, uint64 index2) const {
                if (index1 >= buffLength || index2 >= buffLength)
                    return Optional<ArrayList<T>>();
                uint64 start = (index1 < index2) ? index1 : index2;
                uint64 end = (index1 > index2) ? index1 : index2;
                ArrayList tmp(end - start + 1);
                tmp.SetHardCapacity(false);
                for (uint64 i = start; i <= end; ++i)
                    tmp += buffBuffer[i];
                return Optional<ArrayList<T>>(tmp);
            }
            Optional<Core::Array<T>> atArray(uint64 index1,uint64 index2)const{
                auto opt = at(index1,index2);
                if(!opt) 
                    return Optional<Array<T>>();
                return Optional<Array<T>>(Core::Array<T>(Utilitys::move(opt.value())));
            }
            bool isEmpty() const {
                return buffLength == 0;
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
            Optional<ArrayList<T>> cut(uint64 index1, uint64 index2) {
                auto tmp = at(index1, index2);
                erase(index1, index2);
                return tmp;
            }
            Optional<Core::Array<T>> cutArray(uint64 index1, uint64 index2) {
                auto tmp = cut(index1,index2);
                if(!tmp)
                    return Optional<Core::Array<T>>();
                return Optional<Core::Array<T>>(Core::Array<T>(Utilitys::move(tmp.value())));
            }
            Optional<ArrayList<T>> subArrayList(uint64 index, uint64 count) const {
                if (index >= buffLength)
                    return Optional<ArrayList<T>>();
                if (index + count > buffLength)
                    count = buffLength - index;
                ArrayList out(count);
                out.SetHardCapacity(false);
                for (uint64 i = 0; i < count; ++i)
                    out += buffBuffer[index + i];
                return Optional<ArrayList<T>>(out);
            }
            Optional<Core::Array<T>> subArray(uint64 index, uint64 count) const {
                if (index >= buffLength)
                    return Optional<Core::Array<T>>();
                if (index + count > buffLength)
                    count = buffLength - index;
                Core::Array<T> out(count);
                for (uint64 i = 0; i < count; ++i)
                    out += buffBuffer[index + i];
                return Optional<Core::Array<T>>(out);
            }
            void put(const ArrayList& arr, uint64 index) {
                if(this == &arr)
                    return;
                uint64 len = arr.length();
                if (index > buffLength)
                    index = buffLength;
                uint64 newLen = buffLength + len;
                if (newLen > buffCapacity && !hardCap)
                    SetCapacity(newLen);
                if (newLen > buffCapacity)
                    return;
                for (uint64 i = buffLength; i > index; --i) {
                    new (&buffBuffer[i + len - 1]) T(Utilitys::move(buffBuffer[i - 1]));
                    buffBuffer[i - 1].~T();
                }
                for (uint64 i = 0; i < len; ++i)
                    new (&buffBuffer[index + i]) T(arr.buffBuffer[i]);
                buffLength = newLen;
            }
            void put(const T& tmp,uint64 index){
                if(buffLength ==0 || index >= buffLength)
                    return;
                buffBuffer[index] = tmp;
            }
            void resize(uint64 newLen, const T& fill) {
                if (newLen > buffCapacity) {
                    if (hardCap)
                        newLen = buffCapacity;
                    else
                        SetCapacity(newLen);
                }
                for (uint64 i = buffLength; i < newLen; ++i)
                    new (&buffBuffer[i]) T(fill);
                for (uint64 i = newLen; i < buffLength; ++i)
                    buffBuffer[i].~T();
                buffLength = newLen;
            }
            Core::Array<T> toArray(){
                return Core::Array<T>(Utilitys::move(*this));
            }
            bool includes(const T& includedData)const{
                for(uint64 i=0;i<buffLength;i++)
                    if(buffBuffer[i]== includedData)
                        return true;
                return false;
            }
            void shrinkTo(uint64 index){
                if(buffLength==0) 
                    return;
                erase(index,buffLength-1);
            }
            uint64 Count(const T& in)const{
                uint64 out=0;
                for(uint64 i=0;i<buffLength;i++)
                    if(buffBuffer[i] == in)
                        out++;
                return out;
            }
            T* begin() {
                return buffBuffer;
             }
            T* end()   {
                return buffBuffer + buffLength;
            }

            const T* begin() const {
                return buffBuffer;
            }
            const T* end()   const { 
                return buffBuffer + buffLength; 
            }
            //static

    };
}