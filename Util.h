#ifndef UTIL_H
#define UTIL_H

#include<string>
#include<vector>
#include<sstream>
////////////////////////////////////////////////////////////
//TYPEDEFS                                                //
////////////////////////////////////////////////////////////

// a typedef for a pointer to a function used to store functions in a hash map
typedef int (*function_pointer)();

////////////////////////////////////////////////////////////
//enums                                                   //
////////////////////////////////////////////////////////////


// an enumeration to hold constant values associated to the 'Registers'
enum Registers
{
    Unknown = 0, A = 4,B = 2, D = 1, X = 16, Y = 8
};



////////////////////////////////////////////////////////////
//Functions                                               //
////////////////////////////////////////////////////////////


// This moves the pointer forward past any spaces,'\t' or '\n'
void eat_white_space(char*&);

// converts an array of characters to a string object
//std::string convert_char_array_to_string(char*&);

//checks to see if a character is a comment
bool isComment(char p);
// convert a string to upper case
std::string to_uppercase(std::string);
//tries to read a register value from the string
// return Unknown if no register is found
Registers readRegister(const std::string&);
// attempt to read a byte value from the string
unsigned char readByteValue(const std::string&);
//convert a string to a hex number stored as a short
unsigned short stringToHex(const std::string&);
//convert a string to a decimal number stored as a short
unsigned short stringToDec(const std::string&);


////////////////////////////////////////////////////////////
//Classes                                                 //
////////////////////////////////////////////////////////////


// A basic Hash_Map used for storing values to be quickly accessed
// I am creating my own Hash map so i don't have to include
// any external libraries that are not part of the c++ standard
// The template is a little deceiving the Hash map class K can only
// take a string or char array this has to do with the hash system that is
// implemented, this is not a problem for the scope that the HashMap is being
// used in
template<class K,class V>
class HashMap
{
private:
    // generate a hash value from the key
    int hash(K key)
    {
         int h = 50;

        for(int i = 0;key[i]!='\0';i++)
        {
            h^=key[i];
            h++;
        }
        return h;
    }
    V values[512];

public:
    // Hash_Map constructor
    // create a new vector for function pointers
    HashMap()
    {

    }
    // on delete of hash map free memory used by the vector
    ~HashMap()
    {
        delete values;
    }
    //put the function pointer into the vector at the hash value of the key
    void put(K k,V v)
    {
        values[hash(k)] = v;
    }
    // get a value from the hash map associated to the key
    V& get(K key)
    {
        return values[hash(key)];
    }
};
// a class to create tokens from a char array
class Tokenizer
{
private:
    char* p;
    bool end_of_array;
public:
    Tokenizer(char*);
    ~Tokenizer();
    // get the next token using the default delimiters
    std::string next_token();
    // get the next token using a provided delimiter
    std::string next_token(char);
    // get current pointer
    const char* get_pointer() {return p;}
    // set the pointer to a new a value
    void set_pointer(char* npointer){p = npointer;}
    // check to see if there is more tokens to read from the array
    const bool is_end_of_array() {return end_of_array;}
    //set the end of array bool to false;
    void reset_end() {end_of_array = false;}
};













#endif // UTIL_H













