#include "../console.hpp"

namespace Console {
    Core::Array<Core::String> log;
    Core::String tmpstr;
    Core::String inputBuffer;
    const char* name = "<Console>";
    const int nameLength = 9;
    bool newCommand = true;
    
    void init(){
        log.init(100);
        tmpstr.init();
        inputBuffer.init();
    }
    void Execute(const Core::String& com){
        log += com;
        println(log[log.length() -1].buffer());
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
                        Execute(inputBuffer);
                        inputBuffer.clear();
                        Display::NewLine();
                        break;
                    case 0x08:
                        if(inputBuffer.isEmpty())
                            break;
                        inputBuffer.popBack();
                        if(Display::GetPos('x') > nameLength)
                            Display::DeleteCharacterBehinde();
                        break;
                    default:
                        inputBuffer += character;
                        Display::print(msg);
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