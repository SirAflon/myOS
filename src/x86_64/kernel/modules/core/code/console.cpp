#include "../console.hpp"

namespace Console {
    static char inputBuffer[256];
    static int bufferLen = 0;
    bool newCommand = true;
    const char* name = "<Console>";
    const int nameLength = 9;
    Core::Array<Core::String> log;
    Core::String tmpstr;
    void init(){
        log.init(100);
        tmpstr.init();
    }
    void Execute(const char* com){
        log += Core::String(com);
    }
    void KeyBoardOutput(){
        if(IO::KeyboardHasData()){
            uint8 scancode = IO::ReadKeyboard();
            char character = IO::GetCharacter(scancode);
            if(character != 0){
                char msg[2] = {character, '\0'};
                switch(character){
                    case '\n':
                        newCommand = true;
                        inputBuffer[bufferLen] = '\0';
                        Execute(inputBuffer);
                        bufferLen = 0;
                        Display::NewLine();
                        break;
                    case 0x08:
                        if(Display::GetPos('x') > nameLength)
                            Display::DeleteCharacterBehinde();
                        break;
                    default:
                        if(bufferLen < 255){
                            inputBuffer[bufferLen++] = character;
                            inputBuffer[bufferLen] = '\0';
                            Display::print(msg);
                        }
                        break;
                }
            }
        }
        return;
    }
    void run(){
        if(newCommand){
            Display::print(name);
            newCommand = false;
        }
        KeyBoardOutput();
    }
    void println(const char* ch){
        Core::String& pstr = tmpstr;
        pstr += ch;
        //log += pstr;
        Display::println(ch);
        pstr.clear();
    }
    void print(const char* ch){
        tmpstr += ch;
        Display::print(ch);
    }
    void printLog(){/*
        uint64 lng = log.length();
        Core::String str;
        for(uint64 i=0;i<lng;i++){
            str = log[i];
            Display::println(&str);
        }*/
    }
    void NewLine(){
        //log += tmpstr;
        Display::NewLine();   
        tmpstr.clear();     
    }
    void ClearScreen(){
        Display::ClearScreen();
    }
    void ClearLog(){
        //log.clear();
    }
    void SetCursor(int x, int y){
        Display::SetCursor(x,y);
    }
}
// BACKSPACE = 0x08
// Delete = 0x7F