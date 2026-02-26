#include "../console.hpp"

typedef void (*commandFunc)(const Core::String&);
typedef struct{
    const char* name;
    bool isNative=false;
    commandFunc func;
    const char* help;
} CommandEntry;
void cmdClear(const Core::String& args){
    (void) args;
    Console::ClearLog();
}
void cmdEcho(const Core::String& args){
    Console::println(args.buffer());
}
void cmdMkdir(const Core::String& args){
    if(args==""){
        Console::error("please provide a Path");
        return;
    }
    FileSystem::mkdir(args);
    Console::println("created Directory");
}
void cmdMkfile(const Core::String& args){
    if(args==""){
        Console::error("please provide a Path");
        return;
    }
    FileSystem::mkfile(args);
    Console::println("created File");
}
void cmdLS(const Core::String& args){
    if(args==""){
        Console::error("please provide a Path");
        return;
    }
    const Core::Array<Core::String> obj = FileSystem::ls(args);
    for(uint64 i=0;i<obj.length();i++)
        Console::println(obj[i].buffer());
}
Core::Hash::Map<const char*, CommandEntry> commandMap;
namespace Console {
    Core::Array<Core::String> log;
    Core::Array<Core::String> history;
    Core::String tmpstr;
    Core::String inputBuffer;
    static volatile bool newCommand = true;
    const int nameLength = 9;
    char logScroll = 0;
    static const char name[] = "<Console>";
    char historyIndex = -1;
    void init(){
        log.init(100);
        history.init(100);
        tmpstr.init();
        inputBuffer.init();
        newCommand = true;
        commandMap.init(32);
        commandMap.insert("clear", CommandEntry{"clear",true,cmdClear,"Clears the log"});
        commandMap.insert("echo", CommandEntry{"echo",true,cmdEcho,"prints back what it gets"});
        commandMap.insert("ls", CommandEntry{"ls",true,cmdLS,"lists the objects in directory"});
        commandMap.insert("mkdir", CommandEntry{"mkdir",true,cmdMkdir,"creates new Directory"});
        commandMap.insert("mkfile", CommandEntry{"mkfile",true,cmdMkfile,"creates new File"});
    }
    void Execute(Core::String& com){
        log += Core::String("<Execute>") + com;
        if(!com.isEmpty())
            history  += com;
        else{
            RenderLog();
            return;
        }
        historyIndex = -1;
        logScroll = static_cast<char>(log.length());
        com.trim();
        uint64 firstSpace = com.firstIndexOf(' ');
        bool hasSpace = firstSpace<com.length();
        Core::String args = com.cut(firstSpace+1, com.length()-1);
        if(hasSpace)
            com.popBack();
        const char* co = com.buffer();
        CommandEntry* cmd = commandMap.get(co);
        if(cmd){
            cmd->func(args);
            RenderLog();
            return;
        }
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
        logScroll = static_cast<char>(log.length());
        RenderLog();
    }
    void print(const char* ch){
        tmpstr += ch;
        Display::print(ch);
    }
    void RenderLog(){
        Display::ClearScreen();

        const char screenLines = Display::GetHeightInLines();
        char total = static_cast<char>(log.length());
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
        logScroll = static_cast<char>(log.length());
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
    void error(const char* ch){
        print("Error: ");
        println(ch);
    }
}
// BACKSPACE = 0x08
// Delete = 0x7F
// PageUp = 0x49
//PageDown = 0x15
//ArrowUp = 0x48
//ArrowDown = 0x50