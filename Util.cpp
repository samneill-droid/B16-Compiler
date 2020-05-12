#include "Util.h"

using namespace std;


////////////////////////////////////////////////////////////
//Functions                                               //
////////////////////////////////////////////////////////////

// This moves the pointer forward past any empty space's ' ','\t' or '\n'
void eat_white_space(char*& p)
{
    for(;(*p==' ') | (*p=='\t') | (*p=='\n');p++);
}
// checks to see if this char is a comment delimiter
bool isComment(char p)
{
    return p == ';';
}
// convert a string to Upper case letters
string to_uppercase(string data)
{
    string temp = "";

    for(int i = 0;i < data.size();i++)
    {
        temp+= (data[i] >= 'a' && data[i] <= 'z')? data[i] - ('a'-'A'): data[i];
    }
    return temp;
}
// checks to see if the token passed to it is a register
Registers readRegister(const string& token)
{
    if(token.size() > 1) return Unknown;
    string t = to_uppercase(token);
    switch(t[0])
    {
        case 'A' : return A;
        case 'B' : return B;
        case 'D' : return D;
        case 'X' : return X;
        case 'Y' : return Y;
        default : return Unknown;
    }
}

//reads a byte value from a string
unsigned char readByteValue(const std::string& token)
{
    unsigned char val = 0;
    bool isHex = false;
    string sval = "";

    int index = 0;

    if(token[index]=='$')
    {
        index++;
        isHex = true;
    }

    sval = token.substr(index,token.size());

    return (isHex)? (unsigned char)stringToHex(sval) :
                    (unsigned char) stringToDec(sval);

}

// convert a string to a hex value
unsigned short stringToHex(const string& sval)
{
    stringstream ss(sval);
    unsigned short val;
    ss << hex;
    ss >> val;
    return val;
}
//convert a string to a decimal value
unsigned short stringToDec(const string& sval)
{
    stringstream ss(sval);
    unsigned short val;
    ss >> val;
    return val;
}



////////////////////////////////////////////////////////////
//Class Definitions                                       //
////////////////////////////////////////////////////////////


// initialize the Tokenizer with a pointer to a char array
Tokenizer::Tokenizer(char* pointer)
{
    p = pointer;
    end_of_array = false;
}
// on deconstruction of the Tokenizer delete the pointer to the array
Tokenizer::~Tokenizer()
{
    delete p;
}
// the next_token function() will return the next token in the char array
// tokens are separated based on the following delimiters ' ', '\t', '\n', '\0'
// if a '\0' is reached then the end_of_array flag will be set true
// if a '\'' is reached then a special token is generated to support arrays
string Tokenizer::next_token()
{
    if(end_of_array) throw "Reached end of char array";
    eat_white_space(p);

    if(*p == '\'')
    {
        p++;
        return next_token('\'');
    }

    string token = "";
    for(;(*p != ' ') && (*p != '\t') && (*p != '\0') && (*p != '\n') &&
                (*p != 04);token+=*p++);
    if(*p == '\0') end_of_array = true;

    return token;
}
// the next_token function performs the same task as described above but only
// uses the '\0' and the provided char as delimiters
string Tokenizer::next_token(char delim)
{
    if(end_of_array) throw "Reached end of char array";

    eat_white_space(p);

    string token ="";
    for(;(*p != delim) && (p != '\0');token+=*p++);
    if(*p=='\0') end_of_array = true;

    return token;
}



























