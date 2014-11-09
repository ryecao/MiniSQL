#ifndef BUFFER_MANAGER_
#define BUFFER_MANAGER_

#include <string>
#include <list>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <map>
#include <string>
#include <fstream>

class BufferManager {    
public:
    static const int BufferSize = 100;
    BufferManager();
    ~BufferManager();
    Block AllocateNewBlock(std::string fname);   //allocate a new block in file '$fname'
    Block GetBlock(const std::string &fname,int offset);  //return the block in file '${fname}' start at ${pos}
    void WriteBlock(const Block &);               //save the block's data
    void FreeBlock(const Block &);                //delete a block
    void ClearFile(const std::string &fname);   //delete all block in file fname
    void PinBlock(const Block &);                   
    void ReleasePinBlock(const Block &);
    
private:
    //depende on implementation
    std::list<Block> pool;
    FreeBlockManager fm;    
    Block AddBlock(const std::string &fname,int offset);
    void Flush();
    void WriteBack(const Block &b);
};
#endif
