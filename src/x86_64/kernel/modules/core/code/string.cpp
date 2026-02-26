#include "../string.hpp"
namespace Core {
    //Constructor
    String::String()
    :strBuffer(nullptr),
    strLength(0),
    strCapacity(0),
    hardCap(false){};

    void String::init(){
        pinit("",0,0);
    }
    void String::init(uint64 cap){
        hardCap=true;
        pinit("",0,cap);
    }
    void String::init(const char* ch){
        uint64 len = CalcLength(ch);
        pinit(ch,len,len);
    }
    void String::init(const char* ch,uint64 cap){
        hardCap = true;
        uint64 len = CalcLength(ch);
        if(cap < len)
            cap = len;
        pinit(ch,len,cap);
    }
    String::String(uint64 cap){
        init(cap);
    }
    String::String(const char* in){
        init(in);
    }
    String::String(const char* in, uint64 cap){
        init(in,cap);
    }
    String::String(const String& copy){
        hardCap = copy.hardCap;
        uint64 cap = copy.strLength;
        if(!hardCap)
            cap = (cap==0)?0:cap *2;

        pinit(copy.strBuffer,copy.strLength,cap);
    }
    String::String(String&& move){
        strBuffer = move.strBuffer;
        strLength = move.strLength;
        strCapacity = move.strCapacity;
        hardCap = move.hardCap;

        move.strBuffer = nullptr;
        move.strLength = 0;
        move.strCapacity = 0;
        move.hardCap=false;
    }
    //deconstuctor
    String::~String(){
        if(strBuffer)
            Allocator::Buddy::free(strBuffer);
    }
    // Operations
    String& String::operator=(const String& copy){
        if(this ==&copy)
            return *this;
        if(strBuffer)
            Allocator::Buddy::free(strBuffer);
        hardCap= copy.hardCap;
        pinit(copy.strBuffer,copy.strLength,copy.strCapacity);
        return *this;
    }
    String& String::operator=(String&& move){
        if(this == &move)
            return *this;
        if(strBuffer)
            Allocator::Buddy::free(strBuffer);

        strBuffer = move.strBuffer;
        strLength = move.strLength;
        strCapacity = move.strCapacity;
        hardCap = move.hardCap;
        move.strBuffer = nullptr;
        move.strLength =0;
        move.strCapacity=0;
        move.hardCap=false;
        return *this;
    }
    String& String::operator=(char c){
        clear();
        *this += c;
        return *this;
    }
    String& String::operator=(const char* s){
        uint64 len = CalcLength(s);
        if(len > strCapacity){
            if(hardCap)
                len = strCapacity;
            else
                SetCapacity(len);
        }
        for(uint64 i=0;i<len;++i)
            strBuffer[i] = s[i];
        strLength = len;
        strBuffer[strLength] = '\0';
        return *this;
    }
    String& String::operator+=(char rhs){
        uint64 newLen = strLength+1;
        if(newLen > strCapacity && !hardCap){
            uint64 newCap = newLen *2;
            char* newBuffer = (char*)Allocator::Buddy::alloc(newCap+1);
            for(uint64 i=0;i<strLength;++i)
                newBuffer[i] = strBuffer[i];
            if(strBuffer)
                Allocator::Buddy::free(strBuffer);
            strBuffer = newBuffer;
            strCapacity = newCap;
        } else if(newLen > strCapacity){
            return *this;
        }
        strBuffer[strLength] = rhs;
        strLength++;
        strBuffer[strLength] = '\0';
        return *this;
    }
    String& String::operator+=(const char* rhs){
        uint64 rhsLen = CalcLength(rhs);
        uint64 newLen = strLength + rhsLen;
        if(newLen > strCapacity && !hardCap){
            uint64 newCap = newLen *2;
            char* newBuffer = (char*)Allocator::Buddy::alloc(newCap+1);
            for(uint64 i=0;i<strLength;++i)
                newBuffer[i] = strBuffer[i];
            if(strBuffer)
                Allocator::Buddy::free(strBuffer);
            strBuffer = newBuffer;
            strCapacity = newCap;
        } else if(newLen > strCapacity){
            rhsLen = (strCapacity-strLength);
            newLen = strLength + rhsLen;
        }
        for(uint64 i=0;i<rhsLen;++i)
            strBuffer[strLength + i] = rhs[i];
        strLength = newLen;
        strBuffer[strLength] = '\0';
        return *this;
    }
    String& String::operator+=(const String& add){
        uint64 addLen = add.strLength;
        uint64 newLen = strLength + addLen;
        if(newLen > strCapacity && !hardCap){
            uint64 newCap = newLen *2;
            char* newBuffer = (char*)Allocator::Buddy::alloc(newCap+1);
            for(uint64 i=0;i<strLength;++i)
                newBuffer[i] = strBuffer[i];
            if(strBuffer)
                Allocator::Buddy::free(strBuffer);
            strBuffer = newBuffer;
            strCapacity = newCap;
        } else if(newLen > strCapacity){
            addLen = (strCapacity-strLength);
            newLen = strLength +addLen;
        }
        for (uint64 i=0;i<addLen;++i)
            strBuffer[strLength +i] = add.strBuffer[i];
        strLength = newLen;
        strBuffer[strLength] = '\0';
        return *this;
    }
    String String::operator+(char rhs)const{
        String temp(*this);
        temp += rhs;
        return temp;
    }
    String String::operator+(const char* rhs)const{
        String temp(*this);
        temp += rhs;
        return temp;
    }
    String String::operator+(const String& str)const{
        String temp(*this);
        temp += str;
        return temp;
    }
    char& String::operator[](uint64 index){
        return strBuffer[index];
    }
    const char& String::operator[](uint64 index) const{
        return strBuffer[index];
    }
    bool String::operator==(const String& s)const{
        if(strLength != s.strLength) return false;
        for(uint64 i=0;i<strLength;++i)
            if(strBuffer[i]!=s.strBuffer[i])
                return false;
        return true;
    }
    bool String::operator==(const char* s)const{
        Core::String tmp;
        tmp += s;
        return *this == tmp;
    }
    bool String::operator!=(const String& s)const{
        return !(*this == s);
    }
    bool String::operator<(const String& s)const{
        uint64 n = (strLength < s.strLength)? strLength:s.strLength;
        for(uint64 i= 0;i<n;++i){
            if(strBuffer[i] < s[i]) 
                return true;
            if(strBuffer[i] > s[i])
                return false;
        }
        return (strLength < s.strLength);
    }
    bool String::operator>(const String& s)const{
        return s < *this;
    }
    String::operator bool()const{
        return !(strLength==0);
    }
    //privatFunctions
    void String::pinit(const char* str,uint64 len,uint64 cap){
        strBuffer = (char*)Allocator::Buddy::alloc(cap+1);
        if(!strBuffer){
            strLength=7;
            strCapacity=10;
            return;
        }

        for(uint64 i=0;i<len;++i)
            strBuffer[i] = str[i];
        strBuffer[len] = '\0';
        strLength = len;
        strCapacity = cap;
    }
    //publicFunctions
    uint64 String::length() const{
        return strLength;
    }
    uint64 String::capacity() const{
        return strCapacity;
    }
    void String::SetCapacity(uint64 newCap){
        if(hardCap)
            return;
        char* newBuffer = (char*)Allocator::Buddy::alloc(newCap+1);
        for(uint64 i = 0; i < strLength; ++i)
            newBuffer[i] = strBuffer[i];
        newBuffer[strLength] = '\0';
        if(strBuffer)
            Allocator::Buddy::free(strBuffer);
        strBuffer = newBuffer;
        strCapacity = newCap;
    }
    bool String::hardCapacity()const{
        return hardCap;
    }
    void String::SetHardCapacity(bool set){
        hardCap = set;
    }
    void String::SetHardCapacity(uint64 set){
        hardCap=false;
        SetCapacity(set);
        hardCap = true;
    }
    char* String::buffer(){
        return strBuffer;
    }
    const char* String::buffer() const{
        return strBuffer;
    }
    void String::clear(){
        strLength = 0;
        if(strBuffer)
            strBuffer[0] = '\0';
    }
    void String::shrinkToFit(){
        if(strLength == strCapacity)
            return;
        SetCapacity(strLength);
    }
    char String::getAt(uint64 index)const{
        if(index >= strLength)
            return '\0';
        return strBuffer[index];
    }
    String String::getAt(uint64 index1,uint64 index2)const{
        if(index1 >=strLength||index2 >=strLength)
            return String("");
        uint64 start = (index1<index2)?index1:index2;
        uint64 end = (index1>index2)?index1:index2;
        String tmp(end-start+1);
        for(uint64 i=start;i<=end;++i)
            tmp += strBuffer[i];
        return tmp;
    }
    bool String::isEmpty()const{
        return strLength==0;
    }
    bool String::startsWith(const char* prefix) const {
        uint64 len = CalcLength(prefix);
        if (len > strLength) return false;

        for (uint64 i = 0; i < len; ++i)
            if (strBuffer[i] != prefix[i])
                return false;
        return true;
    }
    bool String::endsWith(const char* suffix) const {
        uint64 len = CalcLength(suffix);
        if (len > strLength) return false;
        uint64 start = strLength - len;
        for (uint64 i = 0; i < len; ++i)
            if (strBuffer[start + i] != suffix[i])
                return false;
        return true;
    }
    int String::compare(const String& other) const {
        uint64 minLen = (strLength < other.strLength) ? strLength : other.strLength;
        for (uint64 i = 0; i < minLen; ++i) {
            if (strBuffer[i] < other.strBuffer[i]) return -1;
            if (strBuffer[i] > other.strBuffer[i]) return 1;
        }
        if (strLength < other.strLength) return -1;
        if (strLength > other.strLength) return 1;
        return 0;
    }
    void String::erase(uint64 index){
        if(index >=strLength) return;
        for(uint64 i= index;i<strLength-1;++i)
            strBuffer[i] = strBuffer[i+1];
        strLength--;
        strBuffer[strLength]='\0';
    }
    void String::erase(uint64 index1,uint64 index2){
        if(index1 >=strLength||index2 >=strLength)
            return;
        uint64 start = (index1<index2)?index1:index2;
        uint64 end = (index1>index2)?index1:index2;
        uint64 count = end-start+1;
        for(uint64 i=start;i+count<strLength;++i)
            strBuffer[i] = strBuffer[i+count];
        strLength -= count;
        strBuffer[strLength] = '\0';
    }
    String String::cut(uint64 index1,uint64 index2){
        String tmp = getAt(index1,index2);
        erase(index1,index2);
        return tmp;
    }
    int64 String::find(char c)const{
        for(uint64 i=0;i<strLength;++i)
            if(strBuffer[i]==c)
                return i;
        return -1;
    }
    String String::subString(uint64 index,uint64 count)const{
        if(index >=strLength) 
            return String("");
        if(index+count > strLength)
            count = strLength -index;
        String out(count);
        for(uint64 i=0;i<count;++i)
            out += strBuffer[index+i];
        return out;
    }
    void String::put(String str,uint64 index){
        uint64 len = str.length();
        if(index>strLength)
            index = strLength;
        uint64 newLen = strLength + len;
        if(newLen > strCapacity && !hardCap)
            SetCapacity(newLen);
        if(newLen>strCapacity)
            return;
        for(uint64 i=strLength;i>index;--i)
            strBuffer[i+len-1] = strBuffer[i-1];
        for(uint64 i=0;i<len;++i)
            strBuffer[index+i]=str[i];
        strLength = newLen;
        strBuffer[strLength] ='\0';
    }
    void String::resize(uint64 newLen,char fill = '\0'){
        if(newLen > strCapacity){
            if(hardCap)
                newLen = strCapacity;
            else
                SetCapacity(newLen);
        }
        for(uint64 i=strLength;i<newLen;++i)
            strBuffer[i] = fill;
        strLength = newLen;
        strBuffer[strLength]='\0';
    }
    void String::reserve(uint64 size){
        if(size<strCapacity)
            return;
        SetCapacity(size);
    }
    void String::trim(){
        trimLeft();
        trimRight();
    }
    void String::trimLeft(){
        uint64 i=0;
        while(i<strLength&&(strBuffer[i]==' '||strBuffer[i]=='\t'))
            i++;
        erase(0,i-1);
    }
    void String::trimRight(){
        if(strLength == 0)
            return;
        uint64 i= strLength-1;
        while(i<strLength&&(strBuffer[i]==' '||strBuffer[i]=='\t'))
            i--;
        erase(i+1,strLength-1);
    }
    void String::replace(char o,char n){
        for(uint64 i=0;i<strLength;++i)
            if(strBuffer[i]==o)
                strBuffer[i] = n;
    }
    void String::toUpper(){
        for(uint64 i=0;i<strLength;++i)
            if(strBuffer[i]>='a'&&strBuffer[i]<='z')
                strBuffer[i] -=32;
    }
    void String::toLower(){
        for(uint64 i=0;i<strLength;++i)
            if(strBuffer[i]>='A'&&strBuffer[i]<='Z')
                strBuffer[i] +=32;
    }
    bool String::equalsIgnoreCase(const String& s)const{
        if(strLength != s.strLength)
            return false;
        for(uint64 i=0;i<strLength;++i){
            char a = strBuffer[i];
            char b = s.strBuffer[i];
            if(a>='A'&&a<='Z') a+=32;
            if(b>='A'&&b<='Z') b+=32;
            if(a!=b)
                return false;
        }
        return true;
    }
    uint64 String::hash()const{
        return Hash::FNV1a(strBuffer);
    }
    uint64 String::hash(hashTypes type)const{
        return Hash::hash(type,strBuffer);
    }
    bool String::contains(char c)const{
        return find(c) >= 0;
    }
    void String::popBack(){
        if(strLength == 0)
            return;
        strLength--;
        strBuffer[strLength] = '\0';
    }
    void String::popBack(uint64 sizeToPop){
        if(strLength == 0)
            return;
        if(strLength < sizeToPop)
            sizeToPop = strLength;
        strLength -= sizeToPop;
        strBuffer[strLength] = '\0';
    }
    uint64 String::Count(const char ch){
        uint64 out=0;
        for(uint64 i=0;i<strLength;i++)
            if(strBuffer[i] == ch)
                out++;
        return out;
    }
    uint64 String::lastIndexOf(const char ch)const{
        uint64 lastIndex=0;
        for(uint64 i=0;i<strLength;i++)
            if(strBuffer[i]==ch)
                lastIndex = i;
        return lastIndex;
    }
    uint64 String::firstIndexOf(const char ch)const{
        for(uint64 i=0;i<strLength;i++)
            if(strBuffer[i]==ch)
                return i;
        return 0;
    }
    char* String::begin() {
        return strBuffer;
    }
    char* String::end()   {
        return strBuffer + strLength;
    }

    const char* String::begin() const {
        return strBuffer;
    }
    const char* String::end()   const { 
        return strBuffer + strLength; 
    }
    Core::Array<Core::String> String::split(const char ch)const{
        Core::ArrayList<Core::String> arr(16);
        uint64 last=0;
        for(uint64 i=0;i<strLength;i++){
            if(strBuffer[i] == ch){
                arr += getAt(last,i);
                last = i;
            }
        }
        return Core::Array<Core::String>(arr);
    }

    //static
    uint64 String::CalcLength(const char* str){
        uint64 len = 0;
        while(str[len] != '\0')
            len++;
        return len;
    }
    int String::compare(const char* a,const char* b){
        while (*a && (*a==*b)){
            a++;
            b++;
        }
        return (unsigned char)*a - (unsigned char)*b;
    }
}