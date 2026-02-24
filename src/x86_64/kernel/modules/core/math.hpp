#pragma once
#include "utilitys.hpp"
namespace Core{
    namespace Math{
        template<typename T>
        constexpr T nextPow2(T value){
            if constexpr (!Utilitys::Checks::isNumeric<T>())
                return 0;
            if(value <= 1)
                return 1;
            value--;
            value |= value >>1;
            value |= value >>2;
            value |= value >>4;
            value |= value >>8;
            value |= value >>16;
            if constexpr (sizeof(T) >=8)
                value |= value >>32;
            value++;
            return value;
        }
    }
}