#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <sstream>

#include"Util.h"

using namespace std;

////////////////////////////////////////////////////////////
//prototypes                                              //
////////////////////////////////////////////////////////////
void init();
void parse();
void scanLabels();
void readMneumonic();
void loadFunctions();
void writeInstruct(string,string,int,int);

int i_mov();
int i_add();
int i_sub();
int i_end();
int i_jmp();
int i_xor();
int i_or();
int i_and();
int i_not();
int i_cmp();
////////////////////////////////////////////////////////////
//GLOBAL VARIABLES                                        //
////////////////////////////////////////////////////////////
bool parsing = true;
bool scan_mode = true;
unsigned short address_location;
// pointer to the output file stream that the program will write to
ofstream* output;
// pointer to the Tokenizer which will be used to read in tokens
// and interpret them appropriately
Tokenizer* tokens;
//The Hash_Map used to store pointers to functions stored in the program
// this will allow our compiler to access them at O(1) time
//HashMap<string,int> fm();
HashMap<string,function_pointer> function_map;
HashMap<string,unsigned short> labels;
////////////////////////////////////////////////////////////
//MAIN                                                    //
////////////////////////////////////////////////////////////
int main(int argc,char**args)
{
    //Check to see if an input file and an output file was
    //passed as arguments to the program
    if(args[1] == NULL)
    {
        cerr << "no input file" << endl;
        return -1;
    }else if(args[2] == NULL)
    {
        cerr << "no output file specified" << endl;
        return -1;
    }
    // Check to see if the first file is the specified b16asm file type
    string ext = "";
    string temp(args[1]);
    bool isExt = false;
    for(int i = 0; i < temp.size();i++)
    {
        if(temp[i] == '.')
        {
            i++;
            isExt = true;
        }
        if(isExt) ext+= temp[i];
    }


    // if the ext doesnt equal the b16asm file type then print an error message
    if(ext != "b16asm")
    {
        cerr << "input file is not a B16ASM file" <<  endl << args[1] << endl;
        return -1;
    }
    // attempt to open an input file stream to the input file
    ifstream b16asm_file(args[1]);
    // now check if the stream is open
    // if its not open it will print an error message and quit the program
    if(!b16asm_file.is_open())
    {
        cerr << "Input file could not be opened" << endl;
        return -1;
    }
    //attempt to open an output file stream to the output file
    output = new ofstream(args[2]);
    //check the status of the output stream if it isnt open print
    // an error message and the quit the program
    if(!output->is_open())
    {
        cerr << "Output file could not be opened" << endl;
        return -1;
    }

    // now we will transfer the contents of the stream input a string object
    string temp_storage = "";
    // pulls chars from the stream and stores them into the string until
    // the end of the stream is reached
    for(;b16asm_file.good();temp_storage+=b16asm_file.get());
    temp_storage[temp_storage.size() -1] = '\0';
    //now the tokenizer will created with a pointer to the temp_storage char array
    tokens = new Tokenizer((char*)temp_storage.c_str());



    //The Init function will now be called to setup the rest of the program
    // to begin compiling the file
    init();
    //now that everything is setup the parsing of the code will begin
    parse();

    (*output).flush();

    delete output;

    cout << "B16 FILE WAS COMPILED SUCCESSFULLY" << endl;

    return 0;
}

// calls the load function and writes a signature to the output stream
void init()
{
    loadFunctions();
    // Writes B16 into the start of the file this a signature used to
    // check to see if the .b16 file is genuine
    (*output) << "B16";
    (*output).flush();
}


//loads the functions used to compile the B16 Code into a hash map
// This is will make it a lot easier and faster to compile code
void loadFunctions()
{
    function_map.put("MOV",&i_mov);
    function_map.put("END",&i_end);
    function_map.put("JMP",&i_jmp);
    function_map.put("ADD",&i_add);
    function_map.put("SUB",&i_sub);
    function_map.put("XOR",&i_xor);
    function_map.put("OR",&i_or);
    function_map.put("NOT",&i_not);
    function_map.put("AND",&i_and);
    function_map.put("CMP",&i_cmp);
}
//the parser starts in scan mode this just looks for labels and generates
// a relative address in the byte code
// after the intial scan the parse function will then reset the token pointer
// and begin the process again in a non scan mode which will then generate the
// b16 byte code
void parse()
{
    parsing = true;
    scan_mode = true;
    address_location = 0;
    // save a pointer at the start of the tokenizer's char array
    const char*p = (char*)tokens->get_pointer();

    while(parsing)
    {
        try
        {
            readMneumonic();
        }
        catch (const char* err)
        {
            cerr << err << endl << "Exiting program" << endl;
            return;
        }
    }
    tokens->set_pointer((char*)p);
    scan_mode = false;
    parsing = true;
    tokens->reset_end();
    while(parsing)
    {
        try
        {
            readMneumonic();
        }
        catch (const char* err)
        {
            cerr << err << endl << "Exiting program" << endl;
            return;
        }
    }
}

// reads a token from the tokenizer and checks to see if the token is a LABEL
// if it is a label then it stores its name and location in a hash map
// if its not a label it assumes that it is an instruction
void readMneumonic()
{
    // check to to see if there is any more tokens left in the tokenizer
    if(tokens->is_end_of_array())
    {
        parsing = false;
        return;
    }


    string token = tokens->next_token();

    token = to_uppercase(token);
    // checks to see if this token is a label declaration
    if(token == "LABEL")
    {
        string label_name = tokens->next_token();
        // if were not in scan mode we don't need to store the label
        if(!scan_mode) return;
        labels.put(label_name,address_location);
        //after storing the label we will return
        return;
    }
    // grab the function from the hash map using the token as the key
    // This essentials replaces a switch function that
    function_pointer fp = function_map.get(token);

    //if the function pointer is null then the Instruction doesn't exist in the hash map
    if(fp == NULL)
    {
        // print an error message and then break out of the parsing
        cerr << "Token didn't relate to any known instruction" << endl
             << token << " : UNKNOWN" << endl;
        parsing = false;
        throw "Fatal Error";
    }

    try
    {
        // this is where the magic is we call the function that the token referenced
        // every function will write a different amount of byte code to the steam
        // the address location will track how much byte code has been written to
        // stream this will be relevant when JMP to Labels are called
        address_location+= fp();
    }
    catch (const char* err)
    {
        // print the error
        cerr << err << endl;
        parsing = false;
        throw "Fatal Error";
    }
}


//writes the correct B16 byte code to the output stream
// MOV instruction can have a various formats such as
// Register to Register or Constant to Register moves
// B16 Assembly follows a similar format to intel's x86 assembly format
// MOV A B will move the B registers contents to the A register
// MOV A 16 will move the 16 to register A
int i_mov()
{
    string token = tokens->next_token();
    string token2 = tokens->next_token();
    if(scan_mode) return 4;

    writeInstruct(token,token2,0x01,0x01);

    return 4;
}
// writes the correct B16 byte code to the output stream
// ADD instruction can have a various formats such as
// Register  Register or Constant Register Addition
int i_add()
{
    string token = tokens->next_token();
    string token2 = tokens->next_token();
    if(scan_mode) return 4;

    writeInstruct(token,token2,0x02,0x01);

    return 4;
}
// writes the correct B16 byte code to the output stream
// SUB instruction can have a various formats such as
// Register  Register or Constant Register Subtraction
int i_sub()
{
    string token = tokens->next_token();
    string token2 = tokens->next_token();
    if(scan_mode) return 4;

    writeInstruct(token,token2,0x03,0x01);

    return 4;
}
// When the end instruct is reached it writes the correct byte code for a
// program end instruct and since this would be the end of the b16 program we will
// end parsing
int i_end()
{
    if(scan_mode) return 1;
    *output << (char)0x04;
    parsing = false;
    return 1;
}
// writes the correct B16 byte code to the output stream
// The jmp instruct only has 2 byte codes once for the jmp instruct
// the other is the address to jmp to
int i_jmp()
{
    string token = tokens->next_token();
    if(scan_mode) return 2;
    *output << 0x05;
    *output << labels.get(token);
    return 2;
}
// writes the correct B16 byte code to the output stream
// XOR instruction can have a various formats such as
// Register  Register or Constant Register
// This is an xor bit operation
int i_xor()
{
    string token = tokens->next_token();
    string token2 = tokens->next_token();
    if(scan_mode) return 4;

    writeInstruct(token,token2,0x06,0x01);

    return 4;
}
// writes the correct B16 byte code to the output stream
// OR instruction can have a various formats such as
// Register  Register or Constant Register OR bitwise operation
int i_or()
{
    string token = tokens->next_token();
    string token2 = tokens->next_token();
    if(scan_mode) return 4;

    writeInstruct(token,token2,0x07,0x01);

    return 4;
}
// writes the correct B16 byte code to the output stream
// AND instruction can have a various formats such as
// Register  Register or Constant Register AND bitwise operation
int i_and()
{
    string token = tokens->next_token();
    string token2 = tokens->next_token();
    if(scan_mode) return 4;

    writeInstruct(token,token2,0x08,0x01);

    return 4;
}
// writes the correct B16 byte code to the output stream
// NOT instruction can have a various formats such as
// Register  Register or Constant Register NOT bitwise operation
int i_not()
{
    string token = tokens->next_token();
    string token2 = tokens->next_token();
    if(scan_mode) return 4;

    writeInstruct(token,token2,0x09,0x01);

    return 4;
}
// writes the correct B16 byte code to the output stream
// CMP instruction can have a various formats such as
// Register  Register or Constant Register
// The compare instruction compares two values and sets the correct flags
int i_cmp()
{
    string token = tokens->next_token();
    string token2 = tokens->next_token();
    if(scan_mode) return 4;

    writeInstruct(token,token2,0x0A,0x01);

    return 4;
}

// the writeInstruct writes the correct byte codes and registers or
// constants to the output stream
void writeInstruct(string token,string token2,int instructCode,int secondaryInstructCode)
{
    Registers firstReg = readRegister(token);

    if(firstReg != Unknown)
    {
        Registers secondReg = readRegister(token2);

        if(secondReg != Unknown)
        {
            *output << (char)instructCode;
            *output << (char)secondaryInstructCode;
            *output << (char)firstReg;
            *output << (char)secondReg;
        }else
        {
            unsigned char byte = (unsigned char)readByteValue(token2);
            *output << (char)instructCode;
            *output << (char)secondaryInstructCode++;
            *output << (char)firstReg;
            *output << byte;
        }
    }else
    {
        throw "Unknown Mov format";
    }
}






















