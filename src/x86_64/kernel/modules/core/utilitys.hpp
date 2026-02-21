#pragma once
#include "dataTypes.hpp"
namespace {
    template<typename T>
    constexpr bool isIntType(){return false;};
    template<>
    constexpr bool isIntType<int>(){return true;};
    template<typename T>
    constexpr bool isDoubleType(){return false;};
    template<>
    constexpr bool isDoubleType<double>(){return true;};
    template<typename T>
    constexpr bool isLongType(){return false;};
    template<>
    constexpr bool isLongType<long>(){return true;};
    template<typename T>
    constexpr bool isShortType(){return false;};
    template<>
    constexpr bool isShortType<short>(){return true;};
    template<typename T>
    constexpr bool isLongLongType(){return false;};
    template<>
    constexpr bool isLongLongType<long long>(){return true;};
    template<typename T>
    constexpr bool isCharType(){return false;};
    template<>
    constexpr bool isCharType<char>(){return true;};
    template<typename T>
    constexpr bool isBoolType(){return false;};
    template<>
    constexpr bool isBoolType<bool>(){return true;};
    template<typename T>
    constexpr bool isFloatType(){return false;};
    template<>
    constexpr bool isFloatType<float>(){return true;};
    template<typename T>
    constexpr bool isLongDoubleType(){return false;};
    template<>
    constexpr bool isLongDoubleType<long double>(){return true;};
    
    template<typename T>
    constexpr bool isUnsingedIntType(){return false;};
    template<>
    constexpr bool isUnsingedIntType<int>(){return true;};
    template<typename T>
    constexpr bool isUnsingedLongType(){return false;};
    template<>
    constexpr bool isUnsingedLongType<long>(){return true;};
    template<typename T>
    constexpr bool isUnsingedShortType(){return false;};
    template<>
    constexpr bool isUnsingedShortType<short>(){return true;};
    template<typename T>
    constexpr bool isUnsingedLongLongType(){return false;};
    template<>
    constexpr bool isUnsingedLongLongType<long long>(){return true;};
    template<typename T>
    constexpr bool isUnsingedCharType(){return false;};
    template<>
    constexpr bool isUnsingedCharType<char>(){return true;};

    template<typename T>
    struct isPrimaryType{static constexpr bool value=false;};
    template <>
    struct isPrimaryType<int>{static constexpr bool value=true;};
    template <>
    struct isPrimaryType<short>{static constexpr bool value=true;};
    template <>
    struct isPrimaryType<long>{static constexpr bool value=true;};
    template <>
    struct isPrimaryType<long long>{static constexpr bool value=true;};
    template <>
    struct isPrimaryType<char>{static constexpr bool value=true;};
    template <>
    struct isPrimaryType<bool>{static constexpr bool value=true;};
    template <>
    struct isPrimaryType<float>{static constexpr bool value=true;};
    template <>
    struct isPrimaryType<double>{static constexpr bool value=true;};
    template <>
    struct isPrimaryType<long double>{static constexpr bool value=true;};

    template<typename T>
    struct typeName{static constexpr const char* value = "Unknown";};
    template<>
    struct typeName<int>{static constexpr const char* value = "int";};
    template<>
    struct typeName<short>{static constexpr const char* value = "short";};
    template<>
    struct typeName<long>{static constexpr const char* value = "long";};
    template<>
    struct typeName<long long>{static constexpr const char* value = "long long";};
    template<>
    struct typeName<char>{static constexpr const char* value = "char";};
    template<>
    struct typeName<bool>{static constexpr const char* value = "bool";};
    template<>
    struct typeName<float>{static constexpr const char* value = "float";};
    template<>
    struct typeName<double>{static constexpr const char* value = "double";};
    template<>
    struct typeName<long double>{static constexpr const char* value = "long double";};

}
namespace Utilitys{
    template<typename T>
    constexpr T&& move(T& t)noexcept{
        return static_cast<T&&>(t);
    }
    template<typename T>
    inline void rawCopy(T* dst, const T* src, uint64 count){
        for(uint64 i=0;i<count;i++)
            dst[i] = src[i];
    }
    template<typename T> struct remove_refrence {using type=T;};
    template<typename T> struct remove_refrence<T&> {using type=T;};
    template<typename T> struct remove_refrence<T&&> {using type=T;};
    template<typename T>
    constexpr T&& forward(typename remove_refrence<T>::type& t)noexcept{
        return static_cast<T&&>(t);
    }
    template<typename T>
    constexpr T&& forward(typename remove_refrence<T>::type&& t)noexcept{
        return static_cast<T&&>(t);
    }
    template<typename T>
    void swap(T& a,T& b) noexcept{
        T tmp = move(a);
        a = move(b);
        b = move(tmp);
    }
    template<typename A,typename B>
    struct isSame{static constexpr bool value = false;};
    template<typename A>
    struct isSame<A,A>{static constexpr bool value = true;};
    template<bool B,typename T = void>
    struct enableIf{};
    template<typename T>
    struct enableIf<true,T>{using type = T;};

    namespace Checks{
        template<typename T>
        constexpr bool isPrimary(){
            return isPrimaryType<T>().value;
        }
        template<typename T>
        constexpr const char* getTypeName(){
            return typeName<T>().value;
        }
        template<typename T>
        constexpr bool isInt(){
            return isIntType<T>();
        }
        template<typename T>
        constexpr bool isDouble(){
            return isDoubleType<T>();
        }
        template<typename T>
        constexpr bool isLong(){
            return isLongType<T>();
        }
        template<typename T>
        constexpr bool isShort(){
            return isShortType<T>();
        }
        template<typename T>
        constexpr bool isLongLong(){
            return isLongLongType<T>();
        }
        template<typename T>
        constexpr bool isChar(){
            return isCharType<T>();
        }
        template<typename T>
        constexpr bool isBool(){
            return isBoolType<T>();
        }
        template<typename T>
        constexpr bool isFloat(){
            return isFloatType<T>();
        }
        template<typename T>
        constexpr bool isLongDouble(){
            return isLongDoubleType<T>();
        }

        template<typename T>
        constexpr bool isUnsingedInt(){
            return isUnsingedIntType<T>();
        }
        template<typename T>
        constexpr bool isUnsingedLong(){
            return isUnsingedLongType<T>();
        }
        template<typename T>
        constexpr bool isUnsingedShort(){
            return isUnsingedShortType<T>();
        }
        template<typename T>
        constexpr bool isUnsingedLongLong(){
            return isUnsingedLongLongType<T>();
        }
        template<typename T>
        constexpr bool isUnsingedChar(){
            return isUnsingedCharType<T>();
        }
    }
}
