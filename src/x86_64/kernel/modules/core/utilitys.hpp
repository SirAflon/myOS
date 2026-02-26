#pragma once
#include "dataTypes.hpp"
#include "allocator.hpp"

// ============================================================================
// Internal type trait helpers (freestanding, no STL)
// ============================================================================

namespace {

// ----------------------
// Signed fundamental types
// ----------------------

template<typename T> constexpr bool isIntType() { return false; }
template<> constexpr bool isIntType<int>() { return true; }

template<typename T> constexpr bool isShortType() { return false; }
template<> constexpr bool isShortType<short>() { return true; }

template<typename T> constexpr bool isLongType() { return false; }
template<> constexpr bool isLongType<long>() { return true; }

template<typename T> constexpr bool isLongLongType() { return false; }
template<> constexpr bool isLongLongType<long long>() { return true; }

template<typename T> constexpr bool isCharType() { return false; }
template<> constexpr bool isCharType<char>() { return true; }

template<typename T> constexpr bool isBoolType() { return false; }
template<> constexpr bool isBoolType<bool>() { return true; }

// ----------------------
// Floating point types
// ----------------------

template<typename T> constexpr bool isFloatType() { return false; }
template<> constexpr bool isFloatType<float>() { return true; }

template<typename T> constexpr bool isDoubleType() { return false; }
template<> constexpr bool isDoubleType<double>() { return true; }

template<typename T> constexpr bool isLongDoubleType() { return false; }
template<> constexpr bool isLongDoubleType<long double>() { return true; }

// ----------------------
// Unsigned types (corrected)
// ----------------------

template<typename T> constexpr bool isUnsignedIntType() { return false; }
template<> constexpr bool isUnsignedIntType<unsigned int>() { return true; }

template<typename T> constexpr bool isUnsignedShortType() { return false; }
template<> constexpr bool isUnsignedShortType<unsigned short>() { return true; }

template<typename T> constexpr bool isUnsignedLongType() { return false; }
template<> constexpr bool isUnsignedLongType<unsigned long>() { return true; }

template<typename T> constexpr bool isUnsignedLongLongType() { return false; }
template<> constexpr bool isUnsignedLongLongType<unsigned long long>() { return true; }

template<typename T> constexpr bool isUnsignedCharType() { return false; }
template<> constexpr bool isUnsignedCharType<unsigned char>() { return true; }

// ----------------------
// Primary type trait
// ----------------------

template<typename T>
struct isPrimaryType { static constexpr bool value = false; };

template<> struct isPrimaryType<int> { static constexpr bool value = true; };
template<> struct isPrimaryType<short> { static constexpr bool value = true; };
template<> struct isPrimaryType<long> { static constexpr bool value = true; };
template<> struct isPrimaryType<long long> { static constexpr bool value = true; };
template<> struct isPrimaryType<char> { static constexpr bool value = true; };
template<> struct isPrimaryType<bool> { static constexpr bool value = true; };
template<> struct isPrimaryType<float> { static constexpr bool value = true; };
template<> struct isPrimaryType<double> { static constexpr bool value = true; };
template<> struct isPrimaryType<long double> { static constexpr bool value = true; };

template<> struct isPrimaryType<unsigned int> { static constexpr bool value = true; };
template<> struct isPrimaryType<unsigned short> { static constexpr bool value = true; };
template<> struct isPrimaryType<unsigned long> { static constexpr bool value = true; };
template<> struct isPrimaryType<unsigned long long> { static constexpr bool value = true; };
template<> struct isPrimaryType<unsigned char> { static constexpr bool value = true; };

// ----------------------
// Type names
// ----------------------

template<typename T>
struct typeName { static constexpr const char* value = "Unknown"; };

template<> struct typeName<int> { static constexpr const char* value = "int"; };
template<> struct typeName<short> { static constexpr const char* value = "short"; };
template<> struct typeName<long> { static constexpr const char* value = "long"; };
template<> struct typeName<long long> { static constexpr const char* value = "long long"; };
template<> struct typeName<char> { static constexpr const char* value = "char"; };
template<> struct typeName<bool> { static constexpr const char* value = "bool"; };
template<> struct typeName<float> { static constexpr const char* value = "float"; };
template<> struct typeName<double> { static constexpr const char* value = "double"; };
template<> struct typeName<long double> { static constexpr const char* value = "long double"; };

template<> struct typeName<unsigned int> { static constexpr const char* value = "unsigned int"; };
template<> struct typeName<unsigned short> { static constexpr const char* value = "unsigned short"; };
template<> struct typeName<unsigned long> { static constexpr const char* value = "unsigned long"; };
template<> struct typeName<unsigned long long> { static constexpr const char* value = "unsigned long long"; };
template<> struct typeName<unsigned char> { static constexpr const char* value = "unsigned char"; };

} // anonymous namespace

// ============================================================================
// Utilitys namespace
// ============================================================================

namespace Utilitys {

// ----------------------
// Global variable storage
// ----------------------

template<typename T>
T* GlobalVariable() {
    static uint8 storage[sizeof(T)];
    static bool init = false;
    if (!init) {
        new (storage) T();
        init = true;
    }
    return reinterpret_cast<T*>(storage);
}

// ----------------------
// Move / forward
// ----------------------

template<typename T>
constexpr T&& move(T& t) noexcept {
    return static_cast<T&&>(t);
}

template<typename T> struct remove_reference { using type = T; };
template<typename T> struct remove_reference<T&> { using type = T; };
template<typename T> struct remove_reference<T&&> { using type = T; };

template<typename T>
constexpr T&& forward(typename remove_reference<T>::type& t) noexcept {
    return static_cast<T&&>(t);
}

template<typename T>
constexpr T&& forward(typename remove_reference<T>::type&& t) noexcept {
    return static_cast<T&&>(t);
}

// ----------------------
// Swap
// ----------------------

template<typename T>
void swap(T& a, T& b) noexcept {
    T tmp = move(a);
    a = move(b);
    b = move(tmp);
}

// ----------------------
// Raw copy
// ----------------------

template<typename T>
inline void rawCopy(T* dst, const T* src, uint64 count) {
    for (uint64 i = 0; i < count; i++)
        dst[i] = src[i];
}

// ----------------------
// isSame
// ----------------------

template<typename A, typename B>
struct isSame { static constexpr bool value = false; };

template<typename A>
struct isSame<A, A> { static constexpr bool value = true; };

// ----------------------
// enableIf
// ----------------------

template<bool B, typename T = void>
struct enableIf {};

template<typename T>
struct enableIf<true, T> { using type = T; };

// ============================================================================
// Checks
// ============================================================================

namespace Checks {

template<typename T>
constexpr bool isNumeric() {
    if (!isPrimaryType<T>::value || isBoolType<T>())
        return false;
    return true;
}

template<typename T> constexpr bool isPrimary() { return isPrimaryType<T>::value; }
template<typename T> constexpr const char* getTypeName() { return typeName<T>::value; }

template<typename T> constexpr bool isInt() { return isIntType<T>(); }
template<typename T> constexpr bool isShort() { return isShortType<T>(); }
template<typename T> constexpr bool isLong() { return isLongType<T>(); }
template<typename T> constexpr bool isLongLong() { return isLongLongType<T>(); }
template<typename T> constexpr bool isChar() { return isCharType<T>(); }
template<typename T> constexpr bool isBool() { return isBoolType<T>(); }

template<typename T> constexpr bool isFloat() { return isFloatType<T>(); }
template<typename T> constexpr bool isDouble() { return isDoubleType<T>(); }
template<typename T> constexpr bool isLongDouble() { return isLongDoubleType<T>(); }

template<typename T> constexpr bool isUnsignedInt() { return isUnsignedIntType<T>(); }
template<typename T> constexpr bool isUnsignedShort() { return isUnsignedShortType<T>(); }
template<typename T> constexpr bool isUnsignedLong() { return isUnsignedLongType<T>(); }
template<typename T> constexpr bool isUnsignedLongLong() { return isUnsignedLongLongType<T>(); }
template<typename T> constexpr bool isUnsignedChar() { return isUnsignedCharType<T>(); }

} // namespace Checks

} // namespace Utilitys
