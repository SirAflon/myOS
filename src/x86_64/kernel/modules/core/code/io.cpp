#include "../io.hpp"
namespace IO {
    static const char scancodeTable[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,   'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,   '\\','z','x','c','v','b','n','m',',','.','/',
    0,   '*', 0,  ' ', 0
};
    bool KeyboardAvailable(){
        return (LowLevelAccess::in8(0x64) & 0x1) != 0;
    }
    bool KeyboardHasData(){
        return LowLevelAccess::in8(0x64) & 1;
    }
    char ReadKeyboard(){
        return LowLevelAccess::in8(0x60);
    }
    char GetCharacter(uint8 scancode){
        if(scancode > 127) return 0;
        return scancodeTable[scancode];
    }
}