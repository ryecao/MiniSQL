#ifndef BUFFER_MANAGER_
#define BUFFER_MANAGER_

#include <string>
#include "block.h"

class BufferManager {    
public:
    BufferManager();

    Block AllocateNewBlock(std::string fname);   //allocate a new block in file '$fname'
    Block GetBlock(std::string fname, int pos);  //return the block in file '${fname}' start at ${pos}
    void WriteBlock(Block &);               //save the block's data
    void FreeBlock(Block &);                //delete a block
    void ClearBlock(const std::string &fname);   //delete all block in file fname

private:
    //depende on implementation
};

#endif
