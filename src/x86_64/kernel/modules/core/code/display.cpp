#include "../display.hpp"
namespace Display{
    static inline volatile uint16* const VGA= (uint16*)0xB8000;
    static int cursorX=0;
    static int cursorY=0;
    void NewLine(){
        cursorX = 0;
        cursorY++;
        if(cursorY >= 25){
            cursorY = 24;
        }
        SetCursor(cursorX, cursorY);
    }
    void print(const char* msg){
        for (int i = 0; msg[i]; i++) {
            char c = msg[i];
            int pos = cursorY * 80 + cursorX;
            VGA[pos] = (0x0F << 8) | c;
            cursorX++;
            if(cursorX >= 80)
                NewLine();
            SetCursor(cursorX, cursorY);
        }
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
        SetCursor(cursorX-1,cursorY);
        print(" ");
        SetCursor(cursorX-1,cursorY);
    }
    void DeleteCharacterBefore(){
        SetCursor(cursorX+1,cursorY);
        print(" ");
        SetCursor(cursorX+1,cursorY);
    }
    int GetPos(char ch){
        return (ch == 'x')? cursorX:cursorY;
    }
}