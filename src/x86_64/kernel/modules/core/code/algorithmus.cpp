#include "../algorithmus.hpp"
namespace Algorithmus{
    constexpr inline uint64 CantorPairing(uint64 a,uint64 b){
        uint64 ab = a+b;
        return (ab*(ab+1))/2 + b;
    }
}