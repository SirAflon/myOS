#include "../display.hpp"
namespace Display{
    static inline volatile uint16* const VGA= (uint16*)0xB8000;
    static int cursorX=0;
    static int cursorY=0;
    static char lines = 24;
    static char charcterInLines = 80;
    void NewLine(){
        cursorX = 0;
        cursorY++;
        if(cursorY >= lines+1){
            cursorY = 24;
        }
        SetCursor(cursorX, cursorY);
    }
    void putChar(char c){
        int pos = cursorY * 80 + cursorX;
        VGA[pos] = (0x0F << 8) | c;
        cursorX++;
        if(cursorX >= 80)
            NewLine();
        SetCursor(cursorX, cursorY);
    }
    void print(const char* msg){
        for (int i = 0; msg[i]; i++) 
            putChar(msg[i]);
    }
    void println(const char* msg){
        print(msg);
        NewLine();
    }
    void print(const Core::String* str){
        print(str->buffer());
    }
    void println(const Core::String* str){
        println(str->buffer());
    }
    void SetCursor(int x, int y){
        cursorX = x;
        cursorY = y;
        int pos = y*80+x;
        LowLevelAccess::out8(0x3D4,(uint8)0x0F);
        LowLevelAccess::out8(0x3D5,(uint8)(pos & 0xFF));
        LowLevelAccess::out8(0x3D4,(uint8)0x0E);
        LowLevelAccess::out8(0x3D5,(uint8)((pos >> 8) & 0xFF));
    }
    void ClearScreen(){
        for (int i = 0; i < 80 * 25; i++) {
            VGA[i] = (0x07 << 8) | ' ';
        }
        SetCursor(0,0);
    }
    void DeleteCharacterBehinde(){
        if(cursorX <= 0)
            return;
        SetCursor(cursorX-1,cursorY);
        print(" ");
        SetCursor(cursorX-1,cursorY);
    }
    void DeleteCharacterBefore(){
        if(cursorX >= 79)
            return;
        SetCursor(cursorX+1,cursorY);
        print(" ");
        SetCursor(cursorX+1,cursorY);
    }
    int GetPos(char ch){
        return (ch == 'x')? cursorX:cursorY;
    }
    void printNumber(uint64 n){
        char buf[32];
        int i = 0;

        if (n == 0){
            putChar('0');
            return;
        }

        while (n > 0){
            buf[i++] = static_cast<char>('0' + (n % 10));
            n /= 10;
        }

        while (i--){
            putChar(buf[i]);
        }
    }
    void printlnNumber(uint64 n){
        printNumber(n);
        NewLine();
    }
    void printHex(uint64 n){
        print("0x");
        bool started = false;

        for (int i = 60; i >= 0; i -= 4){
            uint8 digit = (n >> i) & 0xF;
            if (!started && digit == 0 && i != 0)
                continue;
            started = true;
            if (digit < 10) 
                putChar(static_cast<char>('0' + digit));
            else 
                putChar(static_cast<char>('A' + (digit - 10)));
        }

        if (!started)
            putChar('0');
    }
    void printlnHex(uint64 n){
        printHex(n);
        NewLine();
    }
    char GetHeightInLines(){
        return lines;
    }
    char GetCharacterInLines(){
        return charcterInLines;
    }
    void panic(const char* msg){
        ClearScreen();
        SetCursor(0,0);
        print(msg);
        LowLevelAccess::Hlt();
    }
    void panic(const Core::String* msg){
        ClearScreen();
        SetCursor(0,0);
        print(msg);
        LowLevelAccess::Hlt();
    }
}