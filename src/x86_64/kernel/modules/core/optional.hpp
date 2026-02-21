#pragma once
#include "utilitys.hpp"

namespace Core{
    template<typename T>
    struct Optional{
        private:
            bool mHasValue;
            alignas(T) unsigned char mStorage[sizeof(T)];
            T* ptr(){
                return &mStorage;
            }
            const T* ptr() const{
                return &mStorage;
            }
        public:
            //constructors
            Optional() 
                : mHasValue(false) {}

            Optional(const T& value)
                : mHasValue(true),
                mStorage(new T(value)){}

            Optional(T&& value)
                : mHasValue(true),
                mStorage(new T(Utilitys::move(value))){}

            Optional(const Optional& other)
                : mHasValue(false){
                if(other.ptr()){
                    new(mStorage) T(*other.ptr());
                    mHasValue = true;
                }
            }

            Optional(Optional&& other)
                :mHasValue(false){
                if(other.mHasValue){
                    new(mStorage) T(Utilitys::move(*other.ptr()));
                    mHasValue = true;
                    other.reset();
                }
            }

            ~Optional(){
                reset();
            }
            //operations
            Optional& operator=(const Optional& other){
                if(this == &other)
                    return *this;
                if(mHasValue)
                    ptr()->~T();
                mHasValue = false;
                if(other.mHasValue){
                    new(mStorage) T(*other.ptr());
                    mHasValue=true;
                }
                return *this;
            }
            Optional& operator=(Optional&& other){
                if(this==&other)
                    return *this;
                if(mHasValue)
                    ptr()->~T();
                mHasValue=false;
                if(other.mHasValue){
                    new(mStorage) T(Utilitys::move(*other.ptr()));
                    mHasValue=true;
                    other.reset();
                }
                return *this;
            }
            Optional& operator=(const T& value){
                if(mHasValue)
                    *ptr() = value;
                else{
                    new(mStorage) T(value);
                    mHasValue = true;
                }
                return *this;
            }
            Optional& operator=(T&& value){
                if(mHasValue)
                    *ptr() = Utilitys::move(value);
                else{
                    new (mStorage) T(Utilitys::move(value));
                    mHasValue=true;
                }
                return *this;
            }
            //functions
            bool hasValue() const{
                return mHasValue;
            }
            explicit operator bool()const{
                return mHasValue;
            }
            T& value(){
                return *ptr();
            }
            const T& value()const{
                return *ptr();
            }
            T& operator*(){
                return *ptr();
            }
            const T& operator*()const{
                return *ptr();
            }
            T* operator->(){
                return ptr();
            }
            const T* operator->()const{
                return ptr();
            }
            void reset(){
                if(mHasValue){
                    ptr()->~T();
                    mHasValue=false;
                }
            }
            template<typename... Args>
            void emplace(Args&... args){
                if(mHasValue)
                    ptr()->~T();
                new(mStorage) T(Utilitys::forward<Args>(args)...);
                mHasValue=true;
            }
    };
}