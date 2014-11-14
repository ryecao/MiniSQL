// @copyright (c) 2014 sodabeta
// @license: MIT
// @author(s): sodabeta/rxzxx0723@gmail.com
// created by sodabeta , Nov. , 2014
//
// MiniSQL
// A course project for Database System Design, Fall 2014 @Zhejiang Univ.
//
// @file: block.h
// @brief: block defined used by buffer manager.
//
// please compile with -std=c++11

#ifndef BLOCK_H_
#define BLOCK_H_ 

#include <string>
#include <cstdlib>
#include <cstring>
const int BLOCK_SIZE=4096;
class Block {
public:    
    Block():    pos(0)  {}    
    Block(const Block &o) {
        pos=o.pos,offset=o.offset,is_pined=o.is_pined,is_dirty=o.is_dirty,fname=o.fname;
        memcpy(data,o.data,BLOCK_SIZE);
    }
    Block &operator=(const Block &o) {
        pos=o.pos,offset=o.offset,is_pined=o.is_pined,is_dirty=o.is_dirty,fname=o.fname;
        memcpy(data,o.data,BLOCK_SIZE);   
    }
    Block(const std::string _fname,int _offset):
        fname(_fname),offset(_offset),is_pined(0),is_dirty(0),pos(0) {};    
    int getpos() const { //return the current pointer's pos
        return pos;
    }
    int getoffset() const {
        return offset;
    }
    void show() {
        printf("fname=%s offset=%d\n",fname.c_str(),offset);
        for(int i=0;i<10;++i) if(data[i])
            printf("%d ",i);    puts("");
    }
    int pos;            //the pos in block
    int offset;         //the offset in file
    bool is_pined;
    bool is_dirty;
    unsigned char data[BLOCK_SIZE];
    std::string fname;    
};

#endif
