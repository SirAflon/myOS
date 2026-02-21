#pragma once
#include "dataTypes.hpp"
#include "string.hpp"
#include "comHeader/array+arrayList.hpp"

namespace Core{
    template<typename T>
    struct Span{
        private:
            T* ptr;
            uint64 len;
        public:
            Span()
            :ptr(nullptr),
            len(0){}

            Span(T* data,uint64 length)
            :ptr(data),
            len(length){}
            template<typename U>
            Span(const Core::Array<U>& arr)
            :ptr(arr.buffer()),
            len(arr.length()){}

            template<typename U>
            Span(const Core::ArrayList<U>& list)
            :ptr(list.buffer()),
            len(list.buffer()){}

            Span(const String& str)
            :ptr(str.buffer()),
            len(str.length()){}

            uint64 size()const{
                return len;
            }
            bool empty()const{
                return (len==0);
            }
            T* data(){
                return ptr;
            }
            const T* data()const{
                return ptr;
            }
            T& operator[](uint64 index){
                return ptr[index];
            }
            const T& operator[](uint64 index)const{
                return ptr[index];
            }
            T* begin(){
                return ptr;
            }
            T* end(){
                return ptr + len;
            }
            const T* begin()const{
                return ptr;
            }
            const T* end()const{
                return ptr +len;
            }
            T& front(){
                return ptr[0];
            }
            T& back(){
                return ptr[len-1];
            }
            const T& front()const{
                return ptr[0];
            }
            const T& back()const{
                return ptr[len-1];
            }
            Optional<Span<T>> subSpan(uint64 offset,uint64 count)const{
                if(offset>=len)
                    return Optional<Span<T>>();
                if(offset +count > len)
                    count = len - offset;
                return Optional<Span<T>>(Span<T>(ptr + offset,count));
            }
            Span<T> first(uint64 count)const{
                if(count>len)
                    count = len;
                return Span<T>(ptr,count);
            }
            Span<T> last(uint64 count)const{
                if(count>len)
                    count = len;
                return Span<T>(ptr + (len - count),count);
            }
            operator bool() const{
                return ptr != nullptr && len >0;
            }
    };
}