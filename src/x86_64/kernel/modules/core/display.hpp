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
}