#include "../algorithmus.hpp"
namespace Algorithmus{
    constexpr uint64 CantorPairing(uint64 a,uint64 b){
        return ((a+b)*(a+b+1))/2 + b;
    }
}