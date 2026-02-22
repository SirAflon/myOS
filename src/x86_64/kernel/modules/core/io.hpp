#pragma once
#include "lowLevelAccess.hpp"

namespace IO {
    bool KeyboardHasData();
    bool KeyboardAvailable();
    char ReadKeyboard();
    char GetCharacter(uint8);
}
