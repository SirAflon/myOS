#include "../console.hpp"

namespace Console {
    Core::Array<Core::String> log;
    Core::Array<Core::String> history;
    Core::String tmpstr;
    Core::String inputBuffer;
    const char* name = "<Console>";
    const int nameLength = 9;
    char logScroll = 0;
    char historyIndex = -1;
    bool newCommand = true;
    
    void init(){
        log.init(100);
        history.init(100);
        tmpstr.init();
        inputBuffer.init();
    }
    void Execute(const Core::String& com){
        log += Core::String("<Execute>") + com;
        if(!com.isEmpty())
            history  += com;
        historyIndex = -1;
        logScroll = log.length();
        RenderLog();

    }
    void KeyBoardOutput(){
        if(!IO::KeyboardHasData())
            return;
        uint8 scancode = IO::ReadKeyboard();
        switch(scancode){
            case 0x49:
                if(logScroll>0)
                    logScroll--;
                RenderLog();
                Display::print(name);
                Display::print(inputBuffer.buffer());
                return;
            case 0x51:
                logScroll++;
                RenderLog();
                Display::print(name);
                Display::print(inputBuffer.buffer());
                return;
        }
        char character = IO::GetCharacter(scancode);
        if(character == 0)
            return;
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
        log += pstr;
        Display::println(ch);
        pstr.clear();
        logScroll = log.length();
        RenderLog();
    }
    void print(const char* ch){
        tmpstr += ch;
        Display::print(ch);
    }
    void RenderLog(){
        Display::ClearScreen();

        const char screenLines = Display::GetHeightInLines();
        char total = log.length();
        if(logScroll + screenLines > total)
            logScroll = total > screenLines ? total - screenLines : 0;
        for(char i=0;i<screenLines;i++){
            char idx = logScroll +i;
            if(idx < total)
                Display::println(log[idx].buffer());
            else
                Display::NewLine();
        }
    }
    void NewLine(){
        log += tmpstr;
        Display::NewLine();   
        tmpstr.clear();     
        logScroll = log.length();
        RenderLog();
    }
    void ClearScreen(){
        Display::ClearScreen();
    }
    void ClearLog(){
        log.clear();
    }
    void SetCursor(int x, int y){
        Display::SetCursor(x,y);
    }
}
// BACKSPACE = 0x08
// Delete = 0x7F
// PageUp = 0x49
//PageDown = 0x15
//ArrowUp = 0x48
//ArrowDown = 0x50