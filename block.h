#ifndef BLOCK_H_
#define BLOCK_H_ 

#include <string>

class Block {    
public:    
    Block():pos(0){};
    Block(const Block &); //must be implemented!
    bool operator=(const Block &); //must be overloaded!
    template<class T>
    void append(const T& x); //add x to the end
    void seek(int); //set the pointer's pos
    int getpos() const;  //return the current pointer's pos

    bool is_pined; //pin to prevent to be removed.
    bool is_dirty; //whether the block has been modified
    int pos; //current pointer's pos
    unsigned char data[BLOCK_SIZE]; //data
    std::string fname; //the filename
};

#endif