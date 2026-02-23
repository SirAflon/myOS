#pragma once
#include "lowLevelAccess.hpp"
#include "dataTypes.hpp"
#include "string.hpp"

namespace Display{
    void NewLine();
    void print(const char*);
    void println(const char*);
    void print(const Core::String*);
    void println(const Core::String*);
    void SetCursor(int, int);
    void ClearScreen();
    void DeleteCharacterBehinde();
    void DeleteCharacterBefore();
    int GetPos(char);
    void putChar(char);
    void printNumber(uint64 n);
    void printlnNumber(uint64 n);
    void printHex(uint64 n);
    void printlnHex(uint64 n);
    char GetHeightInLines();
    char GetCharacterInLines();
}