#ifndef BLOCK_H_
#define BLOCK_H_ 

const int BLOCK_SIZE = 4*1024;

class Block {
public:    
    Block():    pos(0)  {}    
    Block(const std::string _fname,int _offset):
        fname(_fname),offset(_offset),is_pined(0),is_dirty(0),pos(0) {};    
    int getpos() const { //return the current pointer's pos
        return pos;
    }   
    int pos;            //the pos in block
    int offset;         //the offset in file
    bool is_pined;
    bool is_dirty;
    unsigned char data[BLOCK_SIZE];
    std::string fname;    
};

#endif
