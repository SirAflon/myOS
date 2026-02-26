#pragma once
#include "display.hpp"
#include "io.hpp"
#include "string.hpp"
#include "allocator.hpp"
#include "comHeader/array+arrayList.hpp"
#include "hashmap.hpp"

namespace Console {
    void run();
    void Execute(Core::String&);
    void println(const char*);
    void print(const char*);
    void NewLine();
    void SetCursor(int x,int y);
    void ClearScreen();
    void ClearLog();
    void RenderLog();
    void init();
    template<typename function>
    void registerCommand(const char* key, function func);
}