#pragma once
#include "lowlevelAccess.hpp"

namespace IO {
    bool KeyboardHasData();
    bool KeyboardAvailable();
    char ReadKeyboard();
    char GetCharacter(uint8);
}
